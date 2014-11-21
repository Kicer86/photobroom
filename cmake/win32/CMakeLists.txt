
# CMake script preparing build environment for windows
# http://stackoverflow.com/questions/17446981/cmake-externalproject-add-and-findpackage

function(install_external_lib)

  set(options)
  set(oneValueArgs NAME)
  set(multiValueArgs DLLFILES)
  cmake_parse_arguments(EXTERNAL_LIB "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )
  
  set(hint)
  
  foreach(lib ${EXTERNAL_LIB_DLLFILES})
    set(LIB_PATH_VAR LIBPATH_${lib})     # name of variable with path to file is combined so it looks nice in CMake's cache file
  
    find_file(${LIB_PATH_VAR} NAMES ${lib}.dll HINTS ${hint} DOC "DLL file location for package build")
    if(${LIB_PATH_VAR})
        install(FILES ${${LIB_PATH_VAR}} DESTINATION ${PATH_LIBS})
        
        #add path of current dll file to hints
        get_filename_component(lib_path ${${LIB_PATH_VAR}} DIRECTORY)
        list(APPEND hint ${lib_path})
        list(REMOVE_DUPLICATES hint)
    else()
        message(FATAL_ERROR "Could not find location for ${lib}.dll file. Set path manuly in CMake's cache file in ${LIB_PATH_VAR} variable.")
    endif()
  endforeach()

endfunction(install_external_lib)


function(addExtraCPackTargets)
                      
    # install compiler's dlls
    set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP TRUE)
    include(InstallRequiredSystemLibraries)
    install(FILES ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS} DESTINATION ${PATH_LIBS})
    
    # install additional ones
    set(libs_OL  putils)
    set(libs_Qt  qt5sql)
    set(libs_SSL libeay32)
            
    install_external_lib(NAME "Open Library" DLLFILES ${libs_OL})
    install_external_lib(NAME Qt5            DLLFILES ${libs_Qt})
    install_external_lib(NAME "Open SSL"     DLLFILES ${libs_SSL})
    
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
                           COMMAND set PATH=${WINDEPLOY_DIR}\;%PATH%                      #without it windeployqt cannot find ICU lib (http://qt-project.org/forums/viewthread/41185)                           
                           COMMAND ${WINDEPLOY} 
                              ARGS --dir ${OUTPUT_PATH}/deploy/tr 
                                   --libdir ${OUTPUT_PATH}/deploy/lib
                                   --no-compiler-runtime 
                                   $<TARGET_FILE:gui>
                                   
                           COMMAND ${CMAKE_COMMAND} -E touch ${OUTPUT_PATH}/deploy_qt5
                           DEPENDS ${OUTPUT_PATH}/deploy_main
                           DEPENDS broom
                           WORKING_DIRECTORY ${WINDEPLOY_DIR}
                          )
    else()
        message(FATAL_ERROR "Could not find windeployqt")
    endif(WINDEPLOY)
    
    #target
    add_custom_target(deploy ALL
                      DEPENDS ${OUTPUT_PATH}/deploy_qt5
                     )
                     
    install(DIRECTORY ${OUTPUT_PATH}/deploy/tr/ DESTINATION ${PATH_BIN})
    install(DIRECTORY ${OUTPUT_PATH}/deploy/lib/ DESTINATION ${PATH_LIBS})
    
endfunction(addExtraCPackTargets)

#execute functions
addExtraCPackTargets()
