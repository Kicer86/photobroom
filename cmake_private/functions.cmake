
#usage:
#addTestTarget(`target` SOURCES source files TEST_LIBRARY gtest|gmock)
#function will add executable with tests and will register it for ctest.

option(ENABLE_SANITIZERS_FOR_TESTS "Enables build of tests with sanitizers turned on" OFF)

macro(addTestTarget target)

    #get sources
    set(multiValueArgs SOURCES LIBRARIES INCLUDES DEFINITIONS)
    cmake_parse_arguments(T "" "" "${multiValueArgs}" ${ARGN} )

    list(APPEND T_DEFINITIONS PRIVATE UNIT_TESTS_BUILD)

    #test_bin_name
    set(test_bin ${target}_tests)

    #add test executables
    add_executable(${test_bin}_base ${T_SOURCES})

    if(ENABLE_SANITIZERS_FOR_TESTS)
        add_executable(${test_bin}_addr ${T_SOURCES})
        add_executable(${test_bin}_thread ${T_SOURCES})
        add_executable(${test_bin}_leak ${T_SOURCES})
        add_executable(${test_bin}_ub ${T_SOURCES})
    endif()

    if(ENABLE_SANITIZERS_FOR_TESTS)
        # setup proper flags for sanitizers
        addFlags(${test_bin}_addr COMPILE_FLAGS "-fsanitize=address")
        addFlags(${test_bin}_addr LINK_FLAGS "-fsanitize=address")

        addFlags(${test_bin}_thread COMPILE_FLAGS "-fsanitize=thread")
        addFlags(${test_bin}_thread LINK_FLAGS "-fsanitize=thread")

        addFlags(${test_bin}_leak COMPILE_FLAGS "-fsanitize=leak")
        addFlags(${test_bin}_leak LINK_FLAGS "-fsanitize=leak")

        addFlags(${test_bin}_ub COMPILE_FLAGS "-fsanitize=undefined -fno-sanitize-recover=all"
                                            "-fsanitize-undefined-trap-on-error"
                                            "-fsanitize=shift "
                                            "-fsanitize=integer-divide-by-zero "
                                            "-fsanitize=unreachable "
                                            "-fsanitize=vla-bound "
                                            "-fsanitize=null "
                                            "-fsanitize=return "
                                            "-fsanitize=signed-integer-overflow "
                                            "-fsanitize=bounds "
                                            "-fsanitize=bounds-strict "
                                            "-fsanitize=alignment "
                                            "-fsanitize=object-size "
                                            "-fsanitize=float-divide-by-zero "
                                            "-fsanitize=float-cast-overflow "
                                            "-fsanitize=nonnull-attribute "
                                            "-fsanitize=returns-nonnull-attribute "
                                            "-fsanitize=bool "
                                            "-fsanitize=enum "
                                            "-fsanitize=vptr"
        )

        addFlags(${test_bin}_ub LINK_FLAGS "-fsanitize=undefined -fno-sanitize-recover=all")
    endif()

    #link against proper libraries
    target_link_libraries(${test_bin}_base PRIVATE ${T_LIBRARIES})

    if(ENABLE_SANITIZERS_FOR_TESTS)
        target_link_libraries(${test_bin}_addr PRIVATE ${T_LIBRARIES})
        target_link_libraries(${test_bin}_thread PRIVATE ${T_LIBRARIES})
        target_link_libraries(${test_bin}_leak PRIVATE ${T_LIBRARIES})
        target_link_libraries(${test_bin}_ub PRIVATE ${T_LIBRARIES})
    endif()

    #include dirs
    target_include_directories(${test_bin}_base ${T_INCLUDES})

    if(ENABLE_SANITIZERS_FOR_TESTS)
        target_include_directories(${test_bin}_addr ${T_INCLUDES})
        target_include_directories(${test_bin}_thread ${T_INCLUDES})
        target_include_directories(${test_bin}_leak ${T_INCLUDES})
        target_include_directories(${test_bin}_ub ${T_INCLUDES})
    endif()

    #definitions
    if(T_DEFINITIONS)
        target_compile_definitions(${test_bin}_base ${T_DEFINITIONS})

        if(ENABLE_SANITIZERS_FOR_TESTS)
            target_compile_definitions(${test_bin}_addr ${T_DEFINITIONS})
            target_compile_definitions(${test_bin}_thread ${T_DEFINITIONS})
            target_compile_definitions(${test_bin}_leak ${T_DEFINITIONS})
            target_compile_definitions(${test_bin}_ub ${T_DEFINITIONS})
        endif()
    endif()

    #enable code coverage
    enableCodeCoverage(${test_bin}_base)

    #add tests
    add_test(${target}_base ${test_bin}_base)
    set(test_binaries ${test_bin}_base)

    if(ENABLE_SANITIZERS_FOR_TESTS)
        add_test(${target}_addr ${test_bin}_addr)
        add_test(${target}_thread ${test_bin}_thread)
        add_test(${target}_leak ${test_bin}_leak)
        add_test(${target}_ub ${test_bin}_ub)

        list(APPEND test_binaries
            ${test_bin}_addr
            ${test_bin}_thread
            ${test_bin}_leak
            ${test_bin}_ub
        )
    endif()

    add_custom_target(${test_bin}
                        DEPENDS
                            ${test_binaries}
    )

endmacro(addTestTarget)


function(disableWarnings target)

    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        target_compile_options(${target} PRIVATE -w)
    endif()

endfunction(disableWarnings)
