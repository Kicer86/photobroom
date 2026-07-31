
find_program(Python NAMES python python3 REQUIRED)

option(ENABLE_SANITIZERS_FOR_TESTS "Enables build of tests with sanitizers turned on" OFF)
option(ENABLE_CODE_COVERAGE "Enables code coeverage for unit tests" OFF)
option(ENABLE_OBJDUMPING "Performs objdump on targets if enabled" OFF)

function(addFlags target propertyName flags)
    get_target_property(current_properties ${target} ${propertyName})

    if(NOT current_properties)
        set(current_properties "")
    endif(NOT current_properties)

    set_target_properties(${target} PROPERTIES ${propertyName} "${current_properties} ${flags}")
endfunction(addFlags)


# helper function for creating clone of an executable target
function(cloneTarget target newtarget)
    add_executable(${newtarget})
    foreach(prp SOURCES LINK_LIBRARIES INCLUDE_DIRECTORIES COMPILE_DEFINITIONS AUTOMOC)
        get_target_property(val ${target} ${prp})
        set_target_properties(${newtarget} PROPERTIES ${prp} "${val}")
    endforeach()
endfunction()


# helper function for creating unit tests with sanitizers enabled
macro(addSanitizers target test_prefix extra_labels)

    #clone target
    cloneTarget(${target} ${target}_addr)
    cloneTarget(${target} ${target}_thread)
    cloneTarget(${target} ${target}_leak)
    cloneTarget(${target} ${target}_ub)

    # setup proper flags for sanitizers
    addFlags(${target}_addr COMPILE_FLAGS "-fsanitize=address")
    addFlags(${target}_addr LINK_FLAGS "-fsanitize=address")

    addFlags(${target}_thread COMPILE_FLAGS "-fsanitize=thread")
    addFlags(${target}_thread LINK_FLAGS "-fsanitize=thread")

    addFlags(${target}_leak COMPILE_FLAGS "-fsanitize=leak")
    addFlags(${target}_leak LINK_FLAGS "-fsanitize=leak")

    addFlags(${target}_ub COMPILE_FLAGS "-fsanitize=undefined -fno-sanitize-recover=all"
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

    addFlags(${target}_ub LINK_FLAGS "-fsanitize=undefined -fno-sanitize-recover=all")

    #add tests
    add_test(${test_prefix}_addr ${target}_addr)
    add_test(${test_prefix}_thread ${target}_thread)
    add_test(${test_prefix}_leak ${target}_leak)
    add_test(${test_prefix}_ub ${target}_ub)

    set_tests_properties(${test_prefix}_addr PROPERTIES LABELS "UnitTest;Sanitizer;Address;${extra_labels}")
    set_tests_properties(${test_prefix}_thread PROPERTIES LABELS "UnitTest;Sanitizer;Thread;${extra_labels}")
    set_tests_properties(${test_prefix}_leak PROPERTIES LABELS "UnitTest;Sanitizer;Leak;${extra_labels}")
    set_tests_properties(${test_prefix}_ub PROPERTIES LABELS "UnitTest;Sanitizer;UndefinedBehavior;${extra_labels}")

endmacro(addSanitizers)


# do some universal setup for a unit test
function(register_unit_test unit_test executable)
    set(extra_labels "${ARGN}")

    set_tests_properties(${unit_test} PROPERTIES LABELS "UnitTest;${extra_labels}")

    if(RUN_TESTS_AFTER_BUILD)
        add_custom_command(TARGET ${executable}
            POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E echo "Running unit test ${unit_test}"  # https://cmake.org/pipermail/cmake/2017-April/065274.html
                COMMAND ${executable} --gtest_brief=1
        )
    endif()

    if(ENABLE_SANITIZERS_FOR_TESTS)
        addSanitizers(${executable} ${unit_test} "${extra_labels}")
    endif()
endfunction()


function(disableWarnings target)

    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        target_compile_options(${target} PRIVATE -w)
    endif()

endfunction(disableWarnings)


function(stringify_file output_file input_file variable_with_type namespace)
    add_custom_command(
        OUTPUT
            ${output_file}
        COMMAND
            ${Python} ${PROJECT_SOURCE_DIR}/cmake/stringify.py
        ARGS
            ${input_file}
            ${output_file}
            ${namespace}
            ${variable_with_type}
        DEPENDS
            ${input_file}
            ${PROJECT_SOURCE_DIR}/cmake/stringify.py
    )

    set_source_files_properties(${output_file} PROPERTIES GENERATED TRUE)

endfunction(stringify_file)


function(convert_svg output_file input_file width height background)
    if(WIN32)
        set(svg_to_image_command
            ${CMAKE_COMMAND} -E env
            "PATH=$<TARGET_FILE_DIR:Qt6::Core>"
            "QT_PLUGIN_PATH=${QT6_INSTALL_PREFIX}/${QT6_INSTALL_PLUGINS}"
            $<TARGET_FILE:svg_to_image>
        )
    else()
        set(svg_to_image_command
            ${CMAKE_COMMAND} -E env
            "QT_QPA_PLATFORM=offscreen"
            $<TARGET_FILE:svg_to_image>
        )
    endif()

    add_custom_command(
        OUTPUT ${output_file}
        COMMAND
            ${svg_to_image_command}
            ${input_file}
            ${output_file}
            ${width}
            ${height}
            ${background}
        DEPENDS
            svg_to_image
            ${input_file}
        VERBATIM
    )
endfunction()


function(objdump_target target)

    if(ENABLE_OBJDUMPING)
        find_program(OBJDUMP objdump REQUIRED)

        set(targetName "objdump_${target}")
        set(lstFile ${PROJECT_BINARY_DIR}/listings/${target}.lst)
        file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/listings)

        add_custom_command(
            OUTPUT ${lstFile}
            COMMAND ${OBJDUMP} -d -M intel --no-addresses --no-show-raw-insn $<TARGET_FILE:${target}> > ${lstFile}
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

        if(target_type STREQUAL "SHARED_LIBRARY")

            set_target_properties(${target} PROPERTIES VERSION ${PROJECT_VERSION}
                                                       SOVERSION ${PROJECT_VERSION_MAJOR})

        endif()

    endif(TARGET ${target})

endfunction(hideSymbols)
