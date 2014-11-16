
#usage:
#addTestTarget(`target` SOURCES source files TEST_LIBRARY gtest|gmock)
#function will add executable with tests and will register it for ctest.

function(addTestTarget target)

    find_package(Threads)

    #get sources
    parseArguments(SOURCES TEST_LIBRARY ARGUMENTS ${ARGN})

    #test_bin_name
    set(test_bin ${target}_test)

    #add test executable
    add_executable(${test_bin} ${SOURCES})

    #find which library should be used
    if("${TEST_LIBRARY}" STREQUAL "GTEST")

        find_package(GTest)
        include_directories(SYSTEM ${GTEST_INCLUDE_DIRS})
        set(link_library ${GTEST_MAIN_LIBRARY} ${GTEST_LIBRARY})

    elseif("${TEST_LIBRARY}" STREQUAL "GMOCK")

        find_package(GTest REQUIRED)
        find_package(GMock REQUIRED)
        include_directories(SYSTEM ${GTEST_INCLUDE_DIRS} ${GMOCK_INCLUDE_DIRS})
        set(link_library ${GMOCK_MAIN_LIBRARY} ${GMOCK_LIBRARY})

    else()
        message(FATAL_ERROR "For 'mode' argument use 'GTEST' or 'GMOCK'. Currently ${mode} was provided")
    endif()

    #link agains test library
    target_link_libraries(${test_bin} ${link_library} ${CMAKE_THREAD_LIBS_INIT})

    #enable code coverage
    enableCodeCoverage(${test_bin})

    #add test
    add_test(${target} ${test_bin})

endfunction(addTestTarget)


macro(find_exif_package)

    find_package(Exiv2)

    if(EXIV2_FOUND)
        set(EXIF_INCLUDE_DIR ${EXIV2_INCLUDE_DIRS})
        set(EXIF_LIBRARIES ${EXIV2_LIBRARIES})
    else()
        find_package(EasyExif REQUIRED)

        set(EXIF_INCLUDE_DIR ${EASYEXIF_INCLUDE_DIRS})
        set(EXIF_LIBRARIES ${EASYEXIF_LIBRARIES})
    endif()

endmacro(find_exif_package)


macro(find_cryptographic_package)

    find_package(OpenSSL)
    
    if(OPENSSL_FOUND)
        set(CRYPTOGRAPHIC_INCLUDE_DIR ${OPENSSL_INCLUDE_DIR})
        set(CRYPTOGRAPHIC_LIBRARIES ${OPENSSL_LIBRARIES})
    else()
        find_package(Nettle REQUIRED)

        set(CRYPTOGRAPHIC_INCLUDE_DIR ${NETTLE_INCLUDE_DIRS})
        set(CRYPTOGRAPHIC_LIBRARIES ${NETTLE_LIBRARIES})
    endif()

endmacro(find_cryptographic_package)

function(addExtraCPackTargets)

    if(WIN32)

        #dir preparations
        add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/deploy_main
                           COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/deploy
                           COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_BINARY_DIR}/deploy_main
                          )

        #Qt5
        find_program(WINDEPLOY windeployqt)
        
        if(WINDEPLOY)

            get_filename_component(WINDEPLOY_DIR ${WINDEPLOY} DIRECTORY)
            add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/deploy_qt5
                               COMMAND set PATH=${WINDEPLOY_DIR}\;%PATH%                      #without it windeployqt cannot find ICU lib (http://qt-project.org/forums/viewthread/41185)
                               COMMAND ${WINDEPLOY} --dir ${CMAKE_BINARY_DIR}/deploy/ $<TARGET_FILE:broom>
                               COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_BINARY_DIR}/deploy_qt5
                               DEPENDS ${CMAKE_BINARY_DIR}/deploy_main
                               DEPENDS broom
                               WORKING_DIRECTORY ${WINDEPLOY_DIR}
                              ) 
        else()
            message(FATAL_ERROR "Could not find windeployqt")
        endif(WINDEPLOY)

        #target
        add_custom_target(deploy ALL
                          DEPENDS ${CMAKE_BINARY_DIR}/deploy_qt5
                         )
    endif(WIN32)

endfunction(addExtraCPackTargets)
