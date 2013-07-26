
project(core)

find_package(OpenLibrary REQUIRED)

include(${OPENLIBRARY_CMAKE_INCLUDES})
include(GenerateExportHeader)

include_directories(${CMAKE_BINARY_DIR}/exports .)

set(CORE_SOURCES
    implementation/types.cpp
    )

set(CORE_HEADERS
    types.hpp
    )

source_group(files REGULAR_EXPRESSION .*core.* )

add_library(core SHARED ${CORE_SOURCES} ${CORE_HEADERS})

generate_export_header(core EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/exports/core_export.h)
turnOnCpp11(core)
turnOnAllWarnings(${CORE_SOURCES})


##################################################################################

if(BUILD_TESTS)

	set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

	find_package(CppUTest REQUIRED)

    include_directories(${CPPUTEST_INCLUDE_DIRS})

	set(TESTS
		tests/coreTests.cpp
		tests/RunAllTests.cpp
		${CORE_SOURCES}
		)

	add_executable(core_tests ${TESTS})
	set_target_properties(core_tests PROPERTIES COMPILE_FLAGS -DCORE_STATIC_DEFINE)
	target_link_libraries(core_tests ${CPPUTEST_LIBRARIES})

	turnOnCpp11(core_tests)

endif(BUILD_TESTS)
