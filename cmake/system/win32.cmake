
# CMake script preparing build environment for windows
# http://stackoverflow.com/questions/17446981/cmake-externalproject-add-and-findpackage

function(install_external_lib)

  set(options)
  set(oneValueArgs NAME LOCATION)
  set(multiValueArgs DLLFILES HINTS)
  cmake_parse_arguments(EXTERNAL_LIB "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

  set(hints ${EXTERNAL_LIB_HINTS})

  if("${EXTERNAL_LIB_LOCATION}" STREQUAL "")
    set(EXTERNAL_LIB_LOCATION ${PATH_LIBS})
  endif()

  foreach(lib ${EXTERNAL_LIB_DLLFILES})
    set(LIB_PATH_VAR LIBPATH_${lib})     # name of variable with path to file is combined so it looks nice in CMake's cache file

    find_file(${LIB_PATH_VAR} NAMES ${lib}.dll HINTS ${hints} DOC "DLL file location for package build")
    if(${LIB_PATH_VAR})
        install(FILES ${${LIB_PATH_VAR}} DESTINATION ${EXTERNAL_LIB_LOCATION})

        #add path of current dll file to hints
        get_filename_component(lib_path ${${LIB_PATH_VAR}} DIRECTORY)
        list(APPEND hints ${lib_path})
        list(REMOVE_DUPLICATES hints)
    else()
        message(FATAL_ERROR "Could not find location for ${lib}.dll file (hints: ${hints}). Set path manually in CMake's cache file in ${LIB_PATH_VAR} variable.")
    endif()
  endforeach()

endfunction(install_external_lib)


macro(addDeploymentActions)

    # install required dll files
    set(libs_OL libQtExt)

    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")

        set(libs_Compiler libgcc_s_dw2-1 libstdc++-6 libwinpthread-1 libgomp-1)

    elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")

        find_program(VS_REDIST
                     vcredist_x86
                     DOC "Visual Studio redistributable package installer"
                     HINTS "$ENV{PROGRAMFILES}/Microsoft Visual Studio 12.0/VC/redist/"
                           "$ENV{PROGRAMFILES}/Microsoft Visual Studio 13.0/VC/redist/"
                           "$ENV{PROGRAMFILES}/Microsoft Visual Studio 14.0/VC/redist/"
                    )

        if(VS_REDIST)
            #recipe from: http://www.cmake.org/pipermail/cmake/2012-January/048540.html
            install(PROGRAMS ${VS_REDIST} DESTINATION tmp)

            set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "ExecWait '\\\"$INSTDIR\\\\tmp\\\\vcredist_x86.exe\\\"'")
        else()
            message(WARNING "Could not find Visual Studio redistributable package installer")
        endif()

        set(libs_Compiler )

    endif()

    install_external_lib(NAME "OpenLibrary"  DLLFILES ${libs_OL} HINTS ${CMAKE_INSTALL_PREFIX}/lib)
    install_external_lib(NAME "Compiler"     DLLFILES ${libs_Compiler} LOCATION ".")

    #Qt5
    set(OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})
    find_program(WINDEPLOY windeployqt)

    add_custom_command(OUTPUT ${OUTPUT_PATH}/deploy_main
                       COMMAND ${CMAKE_COMMAND} -E make_directory ${OUTPUT_PATH}/deploy
                       COMMAND ${CMAKE_COMMAND} -E touch ${OUTPUT_PATH}/deploy_main
                      )

    if(WINDEPLOY)

        get_filename_component(WINDEPLOY_DIR ${WINDEPLOY} DIRECTORY)

        if(BUILD_SHARED_LIBS)
            add_custom_command(OUTPUT ${OUTPUT_PATH}/deploy_qt5
                               COMMAND ${WINDEPLOY}
                                  ARGS --dir ${OUTPUT_PATH}/deploy/tr
                                       --libdir ${OUTPUT_PATH}/deploy/lib
                                       --no-compiler-runtime
                                       --release
                                       $<TARGET_FILE:gui>

                               COMMAND ${WINDEPLOY}
                                  ARGS --dir ${OUTPUT_PATH}/deploy/tr
                                       --libdir ${OUTPUT_PATH}/deploy/lib
                                       --no-compiler-runtime
                                       --release
                                       $<TARGET_FILE:sql_backend_base>

                               COMMAND ${WINDEPLOY}
                                  ARGS --dir ${OUTPUT_PATH}/deploy/tr
                                       --libdir ${OUTPUT_PATH}/deploy/lib
                                       --no-compiler-runtime
                                       --release
                                       $<TARGET_FILE:updater>

                               COMMAND ${CMAKE_COMMAND} -E touch ${OUTPUT_PATH}/deploy_qt5
                               DEPENDS ${OUTPUT_PATH}/deploy_main
                               DEPENDS photo_broom
                               WORKING_DIRECTORY ${WINDEPLOY_DIR}
                              )
        else()
            add_custom_command(OUTPUT ${OUTPUT_PATH}/deploy_qt5
                               COMMAND ${WINDEPLOY}
                                  ARGS --dir ${OUTPUT_PATH}/deploy/tr
                                       --libdir ${OUTPUT_PATH}/deploy/lib
                                       --no-compiler-runtime
                                       --release
                                       $<TARGET_FILE:photo_broom>

                               COMMAND ${WINDEPLOY}
                                  ARGS --dir ${OUTPUT_PATH}/deploy/tr
                                       --libdir ${OUTPUT_PATH}/deploy/lib
                                       --no-compiler-runtime
                                       --release
                                       $<TARGET_FILE:sql_backend_base>

                               COMMAND ${CMAKE_COMMAND} -E touch ${OUTPUT_PATH}/deploy_qt5
                               DEPENDS ${OUTPUT_PATH}/deploy_main
                               DEPENDS photo_broom
                               WORKING_DIRECTORY ${WINDEPLOY_DIR}
                              )
        endif()
    else()
        message(FATAL_ERROR "Could not find windeployqt")
    endif(WINDEPLOY)

    #target
    add_custom_target(deploy ALL
                      DEPENDS ${OUTPUT_PATH}/deploy_qt5
                     )

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
