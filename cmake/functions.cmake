

option(ENABLE_SANITIZERS_FOR_TESTS "Enables build of tests with sanitizers turned on" OFF)
option(ENABLE_CODE_COVERAGE "Enables code coeverage for unit tests" OFF)
option(ENABLE_OBJDUMPING "Performs objdump on targets if enabled" OFF)

#usage:
#addTestTarget(`target` SOURCES source files LIBRARIES libraries to link INCLUDES include directories)
#function will add executable with tests and will register it for ctest.
macro(addTestTarget target)

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


function(stringify_file output_file input_file variable_with_type namespace)

    if(${input_file} IS_NEWER_THAN ${output_file})

        file(READ ${input_file} file_content)

        file(WRITE ${output_file} "#pragma once\n\n")

        file(APPEND ${output_file} "namespace ${namespace} {\n")
        file(APPEND ${output_file} "inline ${variable_with_type} = ")
        file(APPEND ${output_file} "R\"(")
        file(APPEND ${output_file} ${file_content})
        file(APPEND ${output_file} ")\";\n")
        file(APPEND ${output_file} "}\n")
    endif()

endfunction(stringify_file)


function(objdump_target target)

    if(ENABLE_OBJDUMPING)
        find_program(OBJDUMP objdump REQUIRED)

        set(targetName "objdump_${target}")
        set(lstFile ${PROJECT_BINARY_DIR}/listings/${target}.lst)
        file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/listings)

        add_custom_command(
            OUTPUT ${lstFile}
            COMMAND ${OBJDUMP} -d -M intel $<TARGET_FILE:${target}> > ${lstFile}
            DEPENDS ${target}
        )

        add_custom_target(
            ${targetName}
            DEPENDS ${lstFile}
        )

        add_dependencies(DumpObjs ${targetName})
    endif()
endfunction()


function(hideSymbols target)

    if(TARGET ${target})

        get_target_property(target_type ${target} TYPE)

        if(NOT target_type STREQUAL "STATIC_LIBRARY")

            set_target_properties(${target} PROPERTIES CXX_VISIBILITY_PRESET hidden
                                            VISIBILITY_INLINES_HIDDEN 1)

        endif()

    endif(TARGET ${target})

endfunction(hideSymbols)

