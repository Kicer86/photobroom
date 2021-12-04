
# CMake script preparing build environment for windows
# http://stackoverflow.com/questions/17446981/cmake-externalproject-add-and-findpackage

function(setup_qt_environment)
    file(MAKE_DIRECTORY ${OUTPUT_PATH}/deploy)

    find_program(WINDEPLOY windeployqt
        HINTS ${qt_bin_dir}
    )

    if(WINDEPLOY)
        get_filename_component(WINDEPLOY_DIR ${WINDEPLOY} DIRECTORY)

        if(BUILD_SHARED_LIBS)
            add_custom_command(OUTPUT ${OUTPUT_PATH}/deploy_qt6
                               COMMAND ${WINDEPLOY}
                                  ARGS --dir ${OUTPUT_PATH}/deploy/libs
                                       --plugindir ${OUTPUT_PATH}/deploy/libs/qt_plugins
                                       $<$<CONFIG:Debug>:--debug>$<$<CONFIG:Release>:--release>
                                       --qmldir ${PROJECT_SOURCE_DIR}/src/gui/desktop/quick_views
                                       $<TARGET_FILE:gui>

                               COMMAND ${WINDEPLOY}
                                  ARGS --dir ${OUTPUT_PATH}/deploy/libs
                                       --plugindir ${OUTPUT_PATH}/deploy/libs/qt_plugins
                                       $<$<CONFIG:Debug>:--debug>$<$<CONFIG:Release>:--release>
                                       $<TARGET_FILE:sql_backend_base>

                               COMMAND ${WINDEPLOY}
                                  ARGS --dir ${OUTPUT_PATH}/deploy/libs
                                       --plugindir ${OUTPUT_PATH}/deploy/libs/qt_plugins
                                       $<$<CONFIG:Debug>:--debug>$<$<CONFIG:Release>:--release>
                                       $<TARGET_FILE:updater>

                               COMMAND ${CMAKE_COMMAND} -E touch ${OUTPUT_PATH}/deploy_qt6
                               DEPENDS photo_broom
                               WORKING_DIRECTORY ${WINDEPLOY_DIR}
                              )
        else()
            add_custom_command(OUTPUT ${OUTPUT_PATH}/deploy_qt6
                               COMMAND ${WINDEPLOY}
                                  ARGS --dir ${OUTPUT_PATH}/deploy/libs
                                       --plugindir ${OUTPUT_PATH}/deploy/libs/qt_plugins
                                       $<$<CONFIG:Debug>:--debug>$<$<CONFIG:Release>:--release>
                                       --qmldir ${PROJECT_SOURCE_DIR}/src/gui/desktop/quick_views
                                       $<TARGET_FILE:photo_broom>

                               COMMAND ${WINDEPLOY}
                                  ARGS --dir ${OUTPUT_PATH}/deploy/libs
                                       --plugindir ${OUTPUT_PATH}/deploy/libs/qt_plugins
                                       $<$<CONFIG:Debug>:--debug>$<$<CONFIG:Release>:--release>
                                       $<TARGET_FILE:sql_backend_base>

                               COMMAND ${CMAKE_COMMAND} -E touch ${OUTPUT_PATH}/deploy_qt6
                               DEPENDS photo_broom
                               WORKING_DIRECTORY ${WINDEPLOY_DIR}
                              )
        endif()
    else()
        message(FATAL_ERROR "Could not find windeployqt")
    endif(WINDEPLOY)
endfunction()


function(install_external_lib)

  set(options OPTIONAL)
  set(oneValueArgs NAME LOCATION)
  set(multiValueArgs DLLFILES HINTS)
  cmake_parse_arguments(EXTERNAL_LIB "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  if(_VCPKG_INSTALLED_DIR)
    set(VCPKG_REL_HINT_DIR "${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/bin")
    set(VCPKG_DBG_HINT_DIR "${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/debug/bin")
  elseif(VCPKG_TRIPLET_DIR)
    set(VCPKG_REL_HINT_DIR "${VCPKG_TRIPLET_DIR}/bin")
    set(VCPKG_DBG_HINT_DIR "${VCPKG_TRIPLET_DIR}/debug/bin")
  endif()

  if("${EXTERNAL_LIB_LOCATION}" STREQUAL "")
    set(EXTERNAL_LIB_LOCATION ${PATH_LIBS})
  endif()

  set(CopiedBinaries)

  foreach(config IN ITEMS Debug Release)

    if(config MATCHES Debug)
      set(hints ${VCPKG_DBG_HINT_DIR})
    else()
      set(hints ${VCPKG_REL_HINT_DIR})
    endif()

    list(APPEND hints ${EXTERNAL_LIB_HINTS})

    foreach(lib ${EXTERNAL_LIB_DLLFILES})
      set(LIB_PATH_VAR "LIBPATH_${lib}_${config}")     # name of variable with path to file is combined so it looks nice in CMake's cache file

      message(DEBUG "Looking for ${lib} in ${hints}")

      find_file(${LIB_PATH_VAR} NAMES ${lib}.dll ${lib}d.dll HINTS ${hints} DOC "DLL file location for package build")

      if(${LIB_PATH_VAR})
          install(FILES ${${LIB_PATH_VAR}} DESTINATION ${EXTERNAL_LIB_LOCATION} CONFIGURATIONS ${config})
      elseif(EXTERNAL_LIB_OPTIONAL OR config MATCHES Debug)         # debug packages are optional
          message(WARNING "Could not find location for OPTIONAL ${lib}.dll file (hints: ${hints}) for configuration: ${config}. Set path manually in CMake's cache file in ${LIB_PATH_VAR} variable.")
          continue()
      else()
          message(FATAL_ERROR "Could not find location for ${lib}.dll file (hints: ${hints}) for configuration: ${config}. Set path manually in CMake's cache file in ${LIB_PATH_VAR} variable.")
      endif()
    endforeach()
  endforeach()

endfunction(install_external_lib)


function(download_tools)
    if(NOT EXISTS tools/ImageMagick-7.1.0-portable-Q16-x64.zip)
        message("Downloading ImageMagick")
        file(DOWNLOAD
            https://download.imagemagick.org/ImageMagick/download/binaries/ImageMagick-7.1.0-portable-Q16-x64.zip tools/ImageMagick-7.1.0-portable-Q16-x64.zip
            SHOW_PROGRESS
        )
        file(ARCHIVE_EXTRACT
            INPUT tools/ImageMagick-7.1.0-portable-Q16-x64.zip
            DESTINATION tools/ImageMagick
        )
    endif()

    install(DIRECTORY ${OUTPUT_PATH}/tools/ImageMagick/
        DESTINATION tools/ImageMagick
    )

    if(NOT EXISTS tools/ffmpeg-release-essentials.7z)
        message("Downloading FFMpeg")
        file(DOWNLOAD
            https://www.gyan.dev/ffmpeg/builds/ffmpeg-release-essentials.7z tools/ffmpeg-release-essentials.7z
            SHOW_PROGRESS
        )
        file(ARCHIVE_EXTRACT
            INPUT tools/ffmpeg-release-essentials.7z
            DESTINATION tools
        )
    endif()

    install(DIRECTORY ${OUTPUT_PATH}/tools/ffmpeg-4.4.1-essentials_build/   # version :/ not nice to have it here
        DESTINATION tools/FFMpeg
    )

    if(NOT EXISTS tools/exiftool-12.35.zip)
        message("Downloading ExifTool")
        file(DOWNLOAD
            https://exiftool.org/exiftool-12.35.zip tools/exiftool-12.35.zip
            SHOW_PROGRESS
        )
        file(ARCHIVE_EXTRACT
            INPUT tools/exiftool-12.35.zip
            DESTINATION tools
        )
    endif()

    install(FILES ${OUTPUT_PATH}/tools/exiftool\(-k\).exe
        DESTINATION tools/ExifTool/
        RENAME exiftool.exe
    )

    if(NOT EXISTS tools/Hugin-2020.0.0-win64.msi)
        message("Downloading Hugin")
        file(DOWNLOAD
            https://netcologne.dl.sourceforge.net/project/hugin/hugin/hugin-2020.0/Hugin-2020.0.0-win64.msi tools/Hugin-2020.0.0-win64.msi
            SHOW_PROGRESS
        )

        file(TO_NATIVE_PATH "${CMAKE_BINARY_DIR}/tools/Hugin-2020.0.0-win64.msi" hugin_source)
        file(TO_NATIVE_PATH "${CMAKE_BINARY_DIR}/tools/Hugin" hugin_destination)
        execute_process(
            COMMAND msiexec /a ${hugin_source} /quiet TARGETDIR=${hugin_destination}
        )
    endif()

    install(FILES ${OUTPUT_PATH}/tools/Hugin/Hugin/bin/align_image_stack.exe
        DESTINATION tools/Hugin
    )

endfunction(download_tools)


macro(addDeploymentActions)

    find_package(OpenSSL)

    # install required dll files
    set(libs_exiv2 exiv2 zlib1 iconv-2)
    set(libs_dlib cublas64_11                               #dlib dependencies
                  cublasLt64_11
                  openblas
                  liblapack
                  libgfortran-5
                  libgcc_s_seh-1
                  libwinpthread-1
                  libquadmath-0
    )
    set(libs_nvidia
        cudnn64_8                                           #required by dlib when compiled with CUDA
    )
    set(libs_openssl                                        #required by github_api for secure connections
        libcrypto-1_1-x64
        libssl-1_1-x64
    )

    set(libs_qt6
        zstd
        pcre2-16
        harfbuzz
        freetype
        brotlidec
        libpng16
        bz2
        brotlicommon
        sqlite3
        jpeg62
    )

    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")

        # include compiler's runtime copied by windeploy in installer as extra install step
        if("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")
            set(redistributable_file_name vc_redist.x64.exe)
        else()
            set(redistributable_file_name vc_redist.x86.exe)
        endif()

        set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "ExecWait '\\\"$INSTDIR\\\\${redistributable_file_name}\\\"'")

        set(libs_Compiler )

    endif()

    install_external_lib(NAME "Exiv2"
                         DLLFILES ${libs_exiv2}
    )

    install_external_lib(NAME "DLIB"
                         DLLFILES ${libs_dlib}
    )


    install_external_lib(NAME "NVidia"
                         DLLFILES ${libs_nvidia}
                         HINTS "C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v11.2/bin"
                         OPTIONAL
    )

    install_external_lib(NAME "OpenSSL"
                         DLLFILES ${libs_openssl}
                         HINTS ${CMAKE_INSTALL_PREFIX}/lib
                               ${OPENSSL_ROOT_DIR}/bin
                         OPTIONAL
    )

    if(qt_moc_path MATCHES "${_VCPKG_INSTALLED_DIR}.*")             # qt comes from vcpkg - include additional libraries
        message("Including Qt's 3rd party libraries")
        install_external_lib(NAME "Qt6_third_party"
                            DLLFILES ${libs_qt6}
        )
    endif()

    install_external_lib(NAME "Compiler"
                         DLLFILES ${libs_Compiler}
                         LOCATION "."
    )

    # hierarchy setup
    set(OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})

    setup_qt_environment()

    #target
    add_custom_target(deploy ALL
        DEPENDS
            photo_broom
            ${OUTPUT_PATH}/deploy_qt6
    )

    # install deployed files to proper locations
    install(DIRECTORY ${OUTPUT_PATH}/deploy/libs/ DESTINATION ${PATH_LIBS})

endmacro(addDeploymentActions)

find_package(Qt6 REQUIRED COMPONENTS Core)
get_property(qt_moc_path TARGET Qt6::moc PROPERTY LOCATION)
get_filename_component(qt_bin_dir ${qt_moc_path} DIRECTORY)

#enable deployment
addDeploymentActions()

#download tools
download_tools()

#uninstall previous version of photo broom
set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)

#define installation locations on windows (remove any references to version - only project name)
set(CPACK_PACKAGE_INSTALL_DIRECTORY     ${PROJECT_NAME})
set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY  ${PROJECT_NAME})

#define Start menu
set(CPACK_NSIS_EXECUTABLES_DIRECTORY ".")
set(CPACK_PACKAGE_EXECUTABLES
    photo_broom "Photo Broom"
)

if(PHOTO_BROOM_BUILD_ID)
    set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}-b${PHOTO_BROOM_BUILD_ID}")
endif()
