
# CMake script preparing build environment for windows
# http://stackoverflow.com/questions/17446981/cmake-externalproject-add-and-findpackage


function(addExtraCPackTargets)

    #dir preparations
    add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/deploy_main
                       COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/deploy
                       COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_BINARY_DIR}/deploy_main
                      )
                      
    #OpenLibrary
    find_package(OpenLibrary COMPONENTS putils)
    get_target_property(loc OpenLibrary::putils LOCATION)
    install(FILES ${loc} DESTINATION ${PATH_LIBS})

    #Qt5
    find_program(WINDEPLOY windeployqt)
    
    if(WINDEPLOY)

        get_filename_component(WINDEPLOY_DIR ${WINDEPLOY} DIRECTORY)
        add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/deploy_qt5
                           #COMMAND set VCINSTALLDIR=$(VCInstallDir)
                           COMMAND set PATH=${WINDEPLOY_DIR}\;%PATH%                      #without it windeployqt cannot find ICU lib (http://qt-project.org/forums/viewthread/41185)
                           COMMAND ${WINDEPLOY} 
                              ARGS --dir ${CMAKE_BINARY_DIR}/deploy/tr 
                                   --libdir ${CMAKE_BINARY_DIR}/deploy/lib
                                   --no-compiler-runtime 
                                   $<TARGET_FILE:photo_broom>
                                   
                           COMMAND ${WINDEPLOY} 
                              ARGS --dir ${CMAKE_BINARY_DIR}/deploy/tr 
                                   --libdir ${CMAKE_BINARY_DIR}/deploy/lib
                                   --no-compiler-runtime 
                                   $<TARGET_FILE:gui>
                                   
                           COMMAND ${WINDEPLOY} 
                              ARGS --dir ${CMAKE_BINARY_DIR}/deploy/tr 
                                   --libdir ${CMAKE_BINARY_DIR}/deploy/lib
                                   --no-compiler-runtime 
                                   $<TARGET_FILE:sql_backend_base>
                                   
                           COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_BINARY_DIR}/deploy_qt5
                           DEPENDS ${CMAKE_BINARY_DIR}/deploy_main
                           DEPENDS broom
                           WORKING_DIRECTORY ${WINDEPLOY_DIR}
                          )
    else()
        message(FATAL_ERROR "Could not find windeployqt")
    endif(WINDEPLOY)
    
    #system
    set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP TRUE)
    include(InstallRequiredSystemLibraries)
    install(FILES ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS} DESTINATION ${PATH_LIBS})

    #target
    add_custom_target(deploy ALL
                      DEPENDS ${CMAKE_BINARY_DIR}/deploy_qt5
                     )

    install(DIRECTORY ${CMAKE_BINARY_DIR}/deploy/tr/ DESTINATION ${PATH_TR})
    install(DIRECTORY ${CMAKE_BINARY_DIR}/deploy/lib/ DESTINATION ${PATH_LIBS})

endfunction(addExtraCPackTargets)

function(addExtraCPackTargets2)
    
    #dir preparations
    add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/deploy_main
                       COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/deploy
                       COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_BINARY_DIR}/deploy_main
                      )
    
    find_program(DEPENDENCY_WALKER depends)
    if(DEPENDENCY_WALKER)
        add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/deploy/list_of_libraries.cvs
                           COMMAND ${DEPENDENCY_WALKER} 
                                   ARGS /c 
                                        /f:1
                                        /oc:${CMAKE_BINARY_DIR}/deploy/list_of_libraries.cvs
                                        $<TARGET_FILE:photo_broom>
                                        || echo "Problems with Dependency Walker"
                           COMMAND echo test
                           DEPENDS ${CMAKE_BINARY_DIR}/deploy_main
                           )
    else()
        message(FATAL_ERROR "Could not find Dependency Walker")
    endif()  
    
    #target
    add_custom_target(deploy ALL
                      DEPENDS ${CMAKE_BINARY_DIR}/deploy/list_of_libraries.cvs
                     )
    
endfunction(addExtraCPackTargets2)


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


function(addExtraCPackTargets3)
    
    #set(OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR})
    
    #dir preparations
    #add_custom_command(OUTPUT ${OUTPUT_DIR}/deploy_main
    #                   COMMAND ${CMAKE_COMMAND} -E make_directory ${OUTPUT_DIR}/deploy
    #                   COMMAND ${CMAKE_COMMAND} -E touch ${OUTPUT_DIR}/deploy_main
    #                  )
                      
    # install compiler's dlls
    set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP TRUE)
    include(InstallRequiredSystemLibraries)
    install(FILES ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS} DESTINATION ${PATH_LIBS})
    
    # install additional one
    set(libs_OL  putils)
    set(libs_Qt  qt5core qt5gui qt5widgets qt5sql icuin52 icuuc52 icudt52 libGLESv2)
    set(libs_SSL libeay32)
            
    install_external_lib(NAME "Open Library" DLLFILES ${libs_OL})
    install_external_lib(NAME Qt5            DLLFILES ${libs_Qt})
    install_external_lib(NAME "Open SSL"     DLLFILES ${libs_SSL})
    
    #target
    #add_custom_target(deploy ALL
    #                  DEPENDS ${OUTPUT_DIR}/deploy_main
    #                 )
    
endfunction(addExtraCPackTargets3)

#execute functions
addExtraCPackTargets3()
