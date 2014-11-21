
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

#execute functions
addExtraCPackTargets2()
