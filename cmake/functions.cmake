
find_program(Python python REQUIRED)

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
macro(addSanitizers target test_prefix)

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

    set_tests_properties(${test_prefix}_addr PROPERTIES LABELS "UnitTest;Sanitizer;Address")
    set_tests_properties(${test_prefix}_thread PROPERTIES LABELS "UnitTest;Sanitizer;Thread")
    set_tests_properties(${test_prefix}_leak PROPERTIES LABELS "UnitTest;Sanitizer;Leak")
    set_tests_properties(${test_prefix}_ub PROPERTIES LABELS "UnitTest;Sanitizer;UndefinedBehavior")

endmacro(addSanitizers)


# do some universal setup for a unit test
function(register_unit_test unit_test executable)
    set_tests_properties(${unit_test} PROPERTIES LABELS "UnitTest")

    if(RUN_TESTS_AFTER_BUILD)
        add_custom_command(TARGET ${executable}
            POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E echo "Running unit test ${unit_test}"  # https://cmake.org/pipermail/cmake/2017-April/065274.html
                COMMAND ${executable} --gtest_brief=1
        )
    endif()

    if(ENABLE_SANITIZERS_FOR_TESTS)
        addSanitizers(${executable} ${unit_test})
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


function(convertSVG output_file input_file width height)
    find_program(Magick magick)

    if(Magick)
        if(${width} EQUAL -1 AND ${height} EQUAL -1)
            set(resize "")
        elseif(${width} EQUAL -1 OR ${height} EQUAL -1)
            message(FATAL_ERROR "Not handled")
        else()
            set(resize -resize ${width}x${height})
        endif()

        get_filename_component(output_file_ext ${output_file} EXT)
        if(${output_file_ext} STREQUAL "png")
            set(output_file "png32:${output_file}")
        endif()

        add_custom_command(OUTPUT ${output_file}
            COMMAND ${Magick} convert ${input_file} ${resize} ${output_file}
            DEPENDS ${input_file}
        )
    else()
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
            DEPENDS ${input_file}
        )
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

