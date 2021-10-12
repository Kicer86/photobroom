
# CMake script preparing build environment for windows
# http://stackoverflow.com/questions/17446981/cmake-externalproject-add-and-findpackage


function(setup_qt_environment)
    find_package(Qt6 REQUIRED COMPONENTS Core)
    get_property(qt_moc_path TARGET Qt6::moc PROPERTY LOCATION)
	get_filename_component(qt_bin_dir ${qt_moc_path} DIRECTORY)
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
                                       $<$<CONFIG:Debug>:--debug>$<$<CONFIG:Release>:--release>
                                       --qmldir ${PROJECT_SOURCE_DIR}/src/gui/desktop/quick_views
                                       $<TARGET_FILE:gui>

                               COMMAND ${WINDEPLOY}
                                  ARGS --dir ${OUTPUT_PATH}/deploy/libs
                                       $<$<CONFIG:Debug>:--debug>$<$<CONFIG:Release>:--release>
                                       $<TARGET_FILE:sql_backend_base>

                               COMMAND ${WINDEPLOY}
                                  ARGS --dir ${OUTPUT_PATH}/deploy/libs
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
                                       $<$<CONFIG:Debug>:--debug>$<$<CONFIG:Release>:--release>
                                       --qmldir ${PROJECT_SOURCE_DIR}/src/gui/desktop/quick_views
                                       $<TARGET_FILE:photo_broom>

                               COMMAND ${WINDEPLOY}
                                  ARGS --dir ${OUTPUT_PATH}/deploy/libs
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


macro(addDeploymentActions)

    find_package(OpenSSL)

    # install required dll files
    set(libs_OL ${CMAKE_IMPORT_LIBRARY_PREFIX}QtExt)
    set(libs_exiv2 exiv2 zlib1 iconv-2)
    set(libs_dlib cublas64_11                              #dlib dependencies
                  cublasLt64_11
                  openblas
                  liblapack
                  libgfortran-5
                  libgcc_s_seh-1
                  libwinpthread-1
                  libquadmath-0
    )
    set(libs_nvidia
        cudnn64_8                                          #required by dlib when compiled with CUDA
    )
    set(libs_openssl libcrypto-1_1-x64 libssl-1_1-x64)               #required by github_api for secure connections

    set(libs_qt6 zstd pcre2-16 harfbuzz freetype brotlidec libpng16 bz2 brotlicommon)

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

    install_external_lib(NAME "OpenLibrary"
                         DLLFILES ${libs_OL}
    )

    install_external_lib(NAME "Exiv2"
                         DLLFILES ${libs_exiv2}
    )

    install_external_lib(NAME "DLIB"
                         DLLFILES ${libs_dlib}
    )


    install_external_lib(NAME "NVidia"
                         DLLFILES ${libs_dlib}
                         HINTS "C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v11.2/bin"
                         OPTIONAL
    )

    install_external_lib(NAME "OpenSSL"
                         DLLFILES ${libs_openssl}
                         HINTS ${CMAKE_INSTALL_PREFIX}/lib
                               ${OPENSSL_ROOT_DIR}/bin
                         OPTIONAL
    )

    install_external_lib(NAME "Qt6_third_party"
                         DLLFILES ${libs_qt6}
    )

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

#enable deployment
addDeploymentActions()

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
