
# CMake script preparing build environment for windows
# http://stackoverflow.com/questions/17446981/cmake-externalproject-add-and-findpackage


function(setup_qt_environment)
    find_program(WINDEPLOY windeployqt)

    if(WINDEPLOY)
        get_filename_component(WINDEPLOY_DIR ${WINDEPLOY} DIRECTORY)

        if(BUILD_SHARED_LIBS)
            add_custom_command(OUTPUT ${OUTPUT_PATH}/deploy_qt5
                               COMMAND ${WINDEPLOY}
                                  ARGS --dir ${OUTPUT_PATH}/deploy/tr
                                       --libdir ${OUTPUT_PATH}/deploy/lib
                                       --no-compiler-runtime
                                       $<$<CONFIG:Debug>:--debug>$<$<CONFIG:Release>:--release>
                                       --qmldir ${PROJECT_SOURCE_DIR}/src/gui/desktop/quick_views
                                       $<TARGET_FILE:gui>

                               COMMAND ${WINDEPLOY}
                                  ARGS --dir ${OUTPUT_PATH}/deploy/tr
                                       --libdir ${OUTPUT_PATH}/deploy/lib
                                       --no-compiler-runtime
                                       $<$<CONFIG:Debug>:--debug>$<$<CONFIG:Release>:--release>
                                       $<TARGET_FILE:sql_backend_base>

                               COMMAND ${WINDEPLOY}
                                  ARGS --dir ${OUTPUT_PATH}/deploy/tr
                                       --libdir ${OUTPUT_PATH}/deploy/lib
                                       --no-compiler-runtime
                                       $<$<CONFIG:Debug>:--debug>$<$<CONFIG:Release>:--release>
                                       $<TARGET_FILE:updater>

                               COMMAND ${CMAKE_COMMAND} -E touch ${OUTPUT_PATH}/deploy_qt5
                               DEPENDS ${OUTPUT_PATH}/deploy_dirs
                               DEPENDS photo_broom
                               WORKING_DIRECTORY ${WINDEPLOY_DIR}
                              )
        else()
            add_custom_command(OUTPUT ${OUTPUT_PATH}/deploy_qt5
                               COMMAND ${WINDEPLOY}
                                  ARGS --dir ${OUTPUT_PATH}/deploy/tr
                                       --libdir ${OUTPUT_PATH}/deploy/lib
                                       --no-compiler-runtime
                                       $<$<CONFIG:Debug>:--debug>$<$<CONFIG:Release>:--release>
                                       --qmldir ${PROJECT_SOURCE_DIR}/src/gui/desktop/quick_views
                                       $<TARGET_FILE:photo_broom>

                               COMMAND ${WINDEPLOY}
                                  ARGS --dir ${OUTPUT_PATH}/deploy/tr
                                       --libdir ${OUTPUT_PATH}/deploy/lib
                                       --no-compiler-runtime
                                       $<$<CONFIG:Debug>:--debug>$<$<CONFIG:Release>:--release>
                                       $<TARGET_FILE:sql_backend_base>

                               COMMAND ${CMAKE_COMMAND} -E touch ${OUTPUT_PATH}/deploy_qt5
                               DEPENDS ${OUTPUT_PATH}/deploy_dirs
                               DEPENDS photo_broom
                               WORKING_DIRECTORY ${WINDEPLOY_DIR}
                              )
        endif()
    else()
        message(FATAL_ERROR "Could not find windeployqt")
    endif(WINDEPLOY)
endfunction()


function(install_external_lib)

  set(options)
  set(oneValueArgs NAME LOCATION)
  set(multiValueArgs DLLFILES HINTS)
  cmake_parse_arguments(EXTERNAL_LIB "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  set(hints ${EXTERNAL_LIB_HINTS})

  if("${EXTERNAL_LIB_LOCATION}" STREQUAL "")
    set(EXTERNAL_LIB_LOCATION ${PATH_LIBS})
  endif()

  set(CopiedBinaries)

  foreach(lib ${EXTERNAL_LIB_DLLFILES})
    set(LIB_PATH_VAR LIBPATH_${lib})     # name of variable with path to file is combined so it looks nice in CMake's cache file

    find_file(${LIB_PATH_VAR} NAMES ${lib}.dll ${lib}d.dll HINTS ${hints} DOC "DLL file location for package build")
    if(${LIB_PATH_VAR})
        install(FILES ${${LIB_PATH_VAR}} DESTINATION ${EXTERNAL_LIB_LOCATION})

        #add path of current dll file to hints
        get_filename_component(lib_path ${${LIB_PATH_VAR}} DIRECTORY)
        list(APPEND hints ${lib_path})
        list(REMOVE_DUPLICATES hints)
    else()
        message(FATAL_ERROR "Could not find location for ${lib}.dll file (hints: ${hints}). Set path manually in CMake's cache file in ${LIB_PATH_VAR} variable.")
    endif()

    if(DEVELOPER_BUILD)
        get_filename_component(lib_filename ${${LIB_PATH_VAR}} NAME)

        add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/bin/${lib_filename}
                           COMMAND ${CMAKE_COMMAND} -E copy ${${LIB_PATH_VAR}} ${CMAKE_BINARY_DIR}/bin/)

        list(APPEND CopiedBinaries ${CMAKE_BINARY_DIR}/bin/${lib}.dll)
    endif()

  endforeach()

  if(DEVELOPER_BUILD)
    add_custom_target(CopyExternalBinariesToBuild_${EXTERNAL_LIB_NAME}
                      DEPENDS ${CopiedBinaries})

    add_dependencies(CopyExternalBinariesToBuild CopyExternalBinariesToBuild_${EXTERNAL_LIB_NAME})
  endif()

endfunction(install_external_lib)


macro(addDeploymentActions)

    if(DEVELOPER_BUILD)
        add_custom_target(CopyExternalBinariesToBuild ALL
            DEPENDS photo_broom
        )
    endif()

    # install required dll files
    set(libs_OL ${CMAKE_IMPORT_LIBRARY_PREFIX}QtExt)
    set(libs_exiv2 exiv2)

    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")

        set(libs_Compiler libgcc_s_dw2-1 libstdc++-6 libwinpthread-1 libgomp-1)

    elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")

        if("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")
            set(redistributable_file_name vcredist_x64)
        else()
            set(redistributable_file_name vcredist_x86)
        endif()

        find_program(VS_REDIST
                     ${redistributable_file_name}
                     DOC "Visual Studio redistributable package installer"
                     HINTS "$ENV{PROGRAMFILES}/Microsoft Visual Studio/2019/Community/VC/Redist/MSVC/*"
                    )

        if(VS_REDIST)
            #recipe from: http://www.cmake.org/pipermail/cmake/2012-January/048540.html
            install(PROGRAMS ${VS_REDIST} DESTINATION tmp)

            set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "ExecWait '\\\"$INSTDIR\\\\tmp\\\\${redistributable_file_name}\\\"'")
        else()
            message(WARNING "Could not find Visual Studio redistributable package installer")
        endif()

        set(libs_Compiler )

    endif()

    get_filename_component(exiv2_lib_dir "${EXIV2_LIBRARY}" DIRECTORY)

    install_external_lib(NAME "OpenLibrary"
                         DLLFILES ${libs_OL}
                         HINTS ${CMAKE_INSTALL_PREFIX}/lib
                               ${OpenLibrary_DIR}/../bin
    )

    install_external_lib(NAME "Exiv2"
                         DLLFILES ${libs_exiv2}
                         HINTS ${CMAKE_INSTALL_PREFIX}/lib
                               ${CMAKE_INSTALL_PREFIX}/bin
                               ${exiv2_lib_dir}/../bin
    )

    install_external_lib(NAME "Compiler"
                         DLLFILES ${libs_Compiler}
                         LOCATION "."
    )

    # hierarchy setup
    set(OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})

    add_custom_command(OUTPUT ${OUTPUT_PATH}/deploy_dirs
                       COMMAND ${CMAKE_COMMAND} -E make_directory ${OUTPUT_PATH}/deploy
                       COMMAND ${CMAKE_COMMAND} -E make_directory ${OUTPUT_PATH}/dlib
                       COMMAND ${CMAKE_COMMAND} -E touch ${OUTPUT_PATH}/deploy_dirs
                      )

    setup_qt_environment()

    #target
    add_custom_target(deploy DEPENDS
                                    ${OUTPUT_PATH}/deploy_qt5)

    # install deployed files to proper locations
    install(DIRECTORY ${OUTPUT_PATH}/deploy/tr/ DESTINATION ${PATH_LIBS})
    install(DIRECTORY ${OUTPUT_PATH}/deploy/lib/ DESTINATION ${PATH_LIBS})

endmacro(addDeploymentActions)

#enable deployment
addDeploymentActions()

#http://public.kitware.com/Bug/print_bug_page.php?bug_id=7829
set(CPACK_NSIS_EXECUTABLES_DIRECTORY ".")

#uninstall previous version of photo broom
set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)

#define installation locations on windows (remove any references to version - only project name)
set(CPACK_PACKAGE_INSTALL_DIRECTORY     ${PROJECT_NAME})
set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY  ${PROJECT_NAME})

#define Start menu
set(CPACK_PACKAGE_EXECUTABLES
    photo_broom "Photo Broom"
)

if(PHOTO_BROOM_BUILD_ID)
    set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}-b${PHOTO_BROOM_BUILD_ID}")
endif()
