
#usage:
#addTestTarget(`target` SOURCES source files TEST_LIBRARY gtest|gmock)
#function will add executable with tests and will register it for ctest.

macro(addTestTarget target)

    #get sources
    set(multiValueArgs SOURCES LIBRARIES INCLUDES)
    cmake_parse_arguments(T "" "" "${multiValueArgs}" ${ARGN} )

    #test_bin_name
    set(test_bin ${target}_tests)

    #add test executables
    add_executable(${test_bin}_base ${T_SOURCES})
    add_executable(${test_bin}_addr ${T_SOURCES})
    add_executable(${test_bin}_thread ${T_SOURCES})
    add_executable(${test_bin}_leak ${T_SOURCES})
    add_executable(${test_bin}_ub ${T_SOURCES})

    # setup proper flags
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

    #link against proper libraries
    target_link_libraries(${test_bin}_base PRIVATE ${T_LIBRARIES})
    target_link_libraries(${test_bin}_addr PRIVATE ${T_LIBRARIES})
    target_link_libraries(${test_bin}_thread PRIVATE ${T_LIBRARIES})
    target_link_libraries(${test_bin}_leak PRIVATE ${T_LIBRARIES})
    target_link_libraries(${test_bin}_ub PRIVATE ${T_LIBRARIES})

    #include dirs
    target_include_directories(${test_bin}_base ${T_INCLUDES})
    target_include_directories(${test_bin}_addr ${T_INCLUDES})
    target_include_directories(${test_bin}_thread ${T_INCLUDES})
    target_include_directories(${test_bin}_leak ${T_INCLUDES})
    target_include_directories(${test_bin}_ub ${T_INCLUDES})

    #enable code coverage
    enableCodeCoverage(${test_bin}_base)

    #add tests
    add_test(${target}_base ${test_bin}_base)
    add_test(${target}_addr ${test_bin}_addr)
    add_test(${target}_thread ${test_bin}_thread)
    add_test(${target}_leak ${test_bin}_leak)
    add_test(${target}_ub ${test_bin}_ub)

    add_custom_target(${test_bin}
                        DEPENDS
                            ${test_bin}_base
                            ${test_bin}_addr
                            ${test_bin}_thread
                            ${test_bin}_leak
                            ${test_bin}_ub
    )

endmacro(addTestTarget)
