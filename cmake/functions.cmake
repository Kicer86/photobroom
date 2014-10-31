
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

    if(NOT EXIV2_FOUND)
        find_package(EasyExif)
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
