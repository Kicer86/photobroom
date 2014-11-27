
# CMake script preparing build environment for windows
# http://stackoverflow.com/questions/17446981/cmake-externalproject-add-and-findpackage

function(install_external_lib)

  set(options)
  set(oneValueArgs NAME LOCATION)
  set(multiValueArgs DLLFILES)
  cmake_parse_arguments(EXTERNAL_LIB "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )
  
  set(hint)

  if("${EXTERNAL_LIB_LOCATION}" STREQUAL "")
    set(EXTERNAL_LIB_LOCATION ${PATH_LIBS})
  endif()
  
  foreach(lib ${EXTERNAL_LIB_DLLFILES})
    set(LIB_PATH_VAR LIBPATH_${lib})     # name of variable with path to file is combined so it looks nice in CMake's cache file
  
    find_file(${LIB_PATH_VAR} NAMES ${lib}.dll HINTS ${hint} DOC "DLL file location for package build")
    if(${LIB_PATH_VAR})
        install(FILES ${${LIB_PATH_VAR}} DESTINATION ${EXTERNAL_LIB_LOCATION})
        
        #add path of current dll file to hints
        get_filename_component(lib_path ${${LIB_PATH_VAR}} DIRECTORY)
        list(APPEND hint ${lib_path})
        list(REMOVE_DUPLICATES hint)
    else()
        message(FATAL_ERROR "Could not find location for ${lib}.dll file. Set path manuly in CMake's cache file in ${LIB_PATH_VAR} variable.")
    endif()
  endforeach()

endfunction(install_external_lib)


function(addDeploymentActions)

    # install required dll files
    set(libs_OL  libputils)
    set(libs_SSL libeay32)

    if(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
        set(libs_Compiler libgcc_s_dw2-1 libstdc++-6 libwinpthread-1 libgomp-1)
    else()
        set(libs_Compiler )
    endif()
            
    install_external_lib(NAME "Open Library" DLLFILES ${libs_OL})
    install_external_lib(NAME "Open SSL"     DLLFILES ${libs_SSL})
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
                                   
                           COMMAND ${CMAKE_COMMAND} -E touch ${OUTPUT_PATH}/deploy_qt5
                           DEPENDS ${OUTPUT_PATH}/deploy_main
                           DEPENDS photo_broom
                           WORKING_DIRECTORY ${WINDEPLOY_DIR}
                          )
    else()
        message(FATAL_ERROR "Could not find windeployqt")
    endif(WINDEPLOY)
    
    #target
    add_custom_target(deploy ALL
                      DEPENDS ${OUTPUT_PATH}/deploy_qt5
                     )
                     
    install(DIRECTORY ${OUTPUT_PATH}/deploy/tr/ DESTINATION ${PATH_LIBS})
    install(DIRECTORY ${OUTPUT_PATH}/deploy/lib/ DESTINATION ${PATH_LIBS})
    
endfunction(addDeploymentActions)

#enable deployment only for gcc
if(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
    #execute functions
    addDeploymentActions()

    #http://public.kitware.com/Bug/print_bug_page.php?bug_id=7829
    set(CPACK_NSIS_EXECUTABLES_DIRECTORY ".")

    #define Start menu
    set(CPACK_PACKAGE_EXECUTABLES
        PhotoBroom "Photo Broom"
       )
endif()
