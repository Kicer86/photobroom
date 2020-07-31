
#usage:
#addTestTarget(`target` SOURCES source files LIBRARIES libraries to link INCLUDES include directories)
#function will add executable with tests and will register it for ctest.

option(ENABLE_SANITIZERS_FOR_TESTS "Enables build of tests with sanitizers turned on" OFF)
option(ENABLE_CODE_COVERAGE "Enables code coeverage for unit tests" OFF)

macro(addTestTarget target)

    find_package(Qt5Core)

    #get sources
    set(multiValueArgs SOURCES LIBRARIES SYSTEM_INCLUDES INCLUDES DEFINITIONS)
    cmake_parse_arguments(T "" "" "${multiValueArgs}" ${ARGN} )

    list(APPEND T_DEFINITIONS PRIVATE UNIT_TESTS_BUILD)

    #verify sources list
    foreach(source_name ${T_SOURCES})
        get_source_file_property(is_generated ${source_name} GENERATED)

        if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${source_name} AND
           NOT ${is_generated})
            message(FATAL_ERROR "File ${source_name} for '${target}' does not exist")
        endif()
    endforeach()

    #test_bin_name
    set(test_bin ${target}_tests)

    #add test executables
    add_executable(${test_bin}_base ${T_SOURCES})
    target_precompile_headers(${test_bin}_base PRIVATE ${PROJECT_SOURCE_DIR}/cmake/googletest_pch.hpp)
    set_target_properties(${test_bin}_base PROPERTIES AUTOMOC TRUE)

    if(ENABLE_CODE_COVERAGE)
        add_executable(${test_bin}_cc ${T_SOURCES})
        set_target_properties(${test_bin}_cc PROPERTIES AUTOMOC TRUE)
    endif()

    if(ENABLE_SANITIZERS_FOR_TESTS)
        add_executable(${test_bin}_addr ${T_SOURCES})
        add_executable(${test_bin}_thread ${T_SOURCES})
        add_executable(${test_bin}_leak ${T_SOURCES})
        add_executable(${test_bin}_ub ${T_SOURCES})

        target_precompile_headers(${test_bin}_addr PRIVATE ${PROJECT_SOURCE_DIR}/cmake/googletest_pch.hpp)
        target_precompile_headers(${test_bin}_thread PRIVATE ${PROJECT_SOURCE_DIR}/cmake/googletest_pch.hpp)
        target_precompile_headers(${test_bin}_leak PRIVATE ${PROJECT_SOURCE_DIR}/cmake/googletest_pch.hpp)
        target_precompile_headers(${test_bin}_ub PRIVATE ${PROJECT_SOURCE_DIR}/cmake/googletest_pch.hpp)

        set_target_properties(${test_bin}_addr PROPERTIES AUTOMOC TRUE)
        set_target_properties(${test_bin}_thread PROPERTIES AUTOMOC TRUE)
        set_target_properties(${test_bin}_leak PROPERTIES AUTOMOC TRUE)
        set_target_properties(${test_bin}_ub PROPERTIES AUTOMOC TRUE)
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

    if(ENABLE_CODE_COVERAGE)
        target_link_libraries(${test_bin}_cc PRIVATE ${T_LIBRARIES})
    endif()

    if(ENABLE_SANITIZERS_FOR_TESTS)
        target_link_libraries(${test_bin}_addr PRIVATE ${T_LIBRARIES})
        target_link_libraries(${test_bin}_thread PRIVATE ${T_LIBRARIES})
        target_link_libraries(${test_bin}_leak PRIVATE ${T_LIBRARIES})
        target_link_libraries(${test_bin}_ub PRIVATE ${T_LIBRARIES})
    endif()

    #include dirs
    target_include_directories(${test_bin}_base PRIVATE ${T_INCLUDES})
    target_include_directories(${test_bin}_base SYSTEM PRIVATE ${T_SYSTEM_INCLUDES})

    if(ENABLE_CODE_COVERAGE)
        target_include_directories(${test_bin}_cc PRIVATE ${T_INCLUDES})
        target_include_directories(${test_bin}_cc SYSTEM PRIVATE ${T_SYSTEM_INCLUDES})
    endif()

    if(ENABLE_SANITIZERS_FOR_TESTS)
        target_include_directories(${test_bin}_addr PRIVATE ${T_INCLUDES})
        target_include_directories(${test_bin}_thread PRIVATE ${T_INCLUDES})
        target_include_directories(${test_bin}_leak PRIVATE ${T_INCLUDES})
        target_include_directories(${test_bin}_ub PRIVATE ${T_INCLUDES})

        target_include_directories(${test_bin}_addr   SYSTEM PRIVATE ${T_SYSTEM_INCLUDES})
        target_include_directories(${test_bin}_thread SYSTEM PRIVATE ${T_SYSTEM_INCLUDES})
        target_include_directories(${test_bin}_leak   SYSTEM PRIVATE ${T_SYSTEM_INCLUDES})
        target_include_directories(${test_bin}_ub     SYSTEM PRIVATE ${T_SYSTEM_INCLUDES})
    endif()

    #definitions
    if(T_DEFINITIONS)
        target_compile_definitions(${test_bin}_base PRIVATE ${T_DEFINITIONS})

        if(ENABLE_CODE_COVERAGE)
            target_compile_definitions(${test_bin}_cc PRIVATE ${T_DEFINITIONS})
        endif()

        if(ENABLE_SANITIZERS_FOR_TESTS)
            target_compile_definitions(${test_bin}_addr PRIVATE ${T_DEFINITIONS})
            target_compile_definitions(${test_bin}_thread PRIVATE ${T_DEFINITIONS})
            target_compile_definitions(${test_bin}_leak PRIVATE ${T_DEFINITIONS})
            target_compile_definitions(${test_bin}_ub PRIVATE ${T_DEFINITIONS})
        endif()
    endif()

    #enable code coverage
    if(ENABLE_CODE_COVERAGE)

        include(CodeCoverage)

        if (NOT TARGET run_unit_tests_code_coverage)
            add_custom_target(run_unit_tests_code_coverage)
        endif()

        setup_target_for_coverage_lcov(NAME ${test_bin}_run_unit_tests_code_coverage
                                       EXECUTABLE ${test_bin}_cc)

        add_dependencies(run_unit_tests_code_coverage ${test_bin}_run_unit_tests_code_coverage)

    endif()

    #add tests
    add_test(${target}_base ${test_bin}_base)
    set_tests_properties(${target}_base PROPERTIES LABELS "UnitTest")
    set(test_binaries ${test_bin}_base)

    if(ENABLE_SANITIZERS_FOR_TESTS)
        add_test(${target}_addr ${test_bin}_addr)
        add_test(${target}_thread ${test_bin}_thread)
        add_test(${target}_leak ${test_bin}_leak)
        add_test(${target}_ub ${test_bin}_ub)

        set_tests_properties(${target}_addr PROPERTIES LABELS "UnitTest;Sanitizer;Address")
        set_tests_properties(${target}_thread PROPERTIES LABELS "UnitTest;Sanitizer;Thread")
        set_tests_properties(${target}_leak PROPERTIES LABELS "UnitTest;Sanitizer;Leak")
        set_tests_properties(${target}_ub PROPERTIES LABELS "UnitTest;Sanitizer;UndefinedBehavior")

        list(APPEND test_binaries
            ${test_bin}_addr
            ${test_bin}_thread
            ${test_bin}_leak
            ${test_bin}_ub
        )
    endif()

    # make sure all test will be build before running them after build
    add_dependencies(RunUnitTests ${test_binaries})

endmacro(addTestTarget)


function(disableWarnings target)

    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        target_compile_options(${target} PRIVATE -w)
    endif()

endfunction(disableWarnings)


function(stringify_enums output input)

    get_filename_component(input_file_name ${input} NAME_WE)
    set(generated_file_name "${input_file_name}.strings.hpp")

    add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${generated_file_name}
                       COMMAND enum_to_string ${CMAKE_CURRENT_SOURCE_DIR}/${input} ${CMAKE_CURRENT_BINARY_DIR}/${generated_file_name}
                       DEPENDS enum_to_string
                       DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${input}
    )

    set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/${generated_file_name} PROPERTIES GENERATED TRUE)

    set(${output} ${CMAKE_CURRENT_BINARY_DIR}/${generated_file_name} PARENT_SCOPE)

endfunction()


function(stringify_file output_file input_file variable_with_type namespace)

    file(READ ${input_file} file_content)

    file(WRITE ${output_file})

    file(APPEND ${output_file} "namespace ${namespace} {\n")
    file(APPEND ${output_file} "inline ${variable_with_type} = ")
    file(APPEND ${output_file} "R\"(")
    file(APPEND ${output_file} ${file_content})
    file(APPEND ${output_file} ")\";\n")
    file(APPEND ${output_file} "}\n")

endfunction(stringify_file)
