
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


function(determine_image_conversion_tool result)
    find_program(Python python)

    if(Python)
        execute_process(
            COMMAND ${Python} ${PROJECT_SOURCE_DIR}/tools/svg2any.py --check-requirements _ _
            RESULT_VARIABLE _result
            OUTPUT_QUIET
            ERROR_QUIET
        )

        if(_result EQUAL 0)        # success
            set(${result} "PYTHON")
            return(PROPAGATE ${result})
        else()
            message(DEBUG "Image conversion: Python found but 'svg2any.py --check-requirements _ _' exited with an error")
        endif()
    else()
        message(DEBUG "Image conversion: Python not found")
    endif()

    find_program(Magick magick)
    if(Magick)
        set(${result} "MAGICK")
        return(PROPAGATE ${result})
    else()
        message(DEBUG "Image conversion: Magick not found. No tool for image conversion found")
    endif()

    set(${result} "NONE")
    return(PROPAGATE ${result})
endfunction()


function(convertSVG output_file input_file width height)

    if(NOT A_PB_IMAGE_CONVERTER)
        determine_image_conversion_tool(converter)
        set(A_PB_IMAGE_CONVERTER ${converter} CACHE INTERNAL "Tool used for converting svg files" FORCE)
    endif()

    if(A_PB_IMAGE_CONVERTER STREQUAL "NONE")
        # Magick executable was not found OR
        # Python executable was not found OR
        # Python was found but there are missing dependencies for tools/svg2any.py
        message(FATAL_ERROR "Neither magick nor python can be used for generation of required images.")
    endif()

    if(A_PB_IMAGE_CONVERTER STREQUAL "PYTHON")
        find_program(Python python REQUIRED)

        if(${width} EQUAL -1 AND ${height} EQUAL -1)
            set(resize "")
        elseif(${width} EQUAL -1 OR ${height} EQUAL -1)
            message(FATAL_ERROR "Not handled")
        else()
            set(resize --width ${width} --height ${height})
        endif()

        add_custom_command(OUTPUT ${output_file}
            COMMAND ${Python} ${PROJECT_SOURCE_DIR}/tools/svg2any.py ${input_file} ${output_file} ${resize}
            DEPENDS ${PROJECT_SOURCE_DIR}/tools/svg2any.py
            DEPENDS ${input_file}
        )
    elseif(A_PB_IMAGE_CONVERTER STREQUAL "MAGICK")
        find_program(Magick magick REQUIRED)

        if(${width} EQUAL -1 AND ${height} EQUAL -1)
            set(resize "")
        elseif(${width} EQUAL -1 OR ${height} EQUAL -1)
            message(FATAL_ERROR "Not handled")
        else()
            set(resize -resize ${width}x${height})
        endif()

        set(output_opts)
        get_filename_component(output_file_ext ${output_file} EXT)
        if(${output_file_ext} STREQUAL ".png")
            set(output_opts "PNG24:")
        endif()

        add_custom_command(OUTPUT ${output_file}
            COMMAND ${CMAKE_COMMAND} -E env NO_AT_BRIDGE=1 ${Magick} ${input_file} ${resize} ${output_opts}${output_file}
            DEPENDS ${input_file}
        )
    else()
        message(FATAL_ERROR "Unexpected value of A_PB_IMAGE_CONVERTER variable: ${A_PB_IMAGE_CONVERTER}")
    endif()
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

    endif(TARGET ${target})

endfunction(hideSymbols)

