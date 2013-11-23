
project(core)

find_package(OpenLibrary REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(Exiv2 REQUIRED)

include(${OPENLIBRARY_CMAKE_INCLUDES})
include(GenerateExportHeader)

include_directories(${Qt5Core_INCLUDE_DIRS} ${EXIV2_INCLUDE_DIRS} ${CMAKE_BINARY_DIR}/exports .)

set(CORE_SOURCES
    implementation/types.cpp
    implementation/tag_feeder.cpp
    )

set(CORE_HEADERS
    types.hpp
    tag_feeder.hpp
    )

source_group(files REGULAR_EXPRESSION .*core.* )

add_library(core SHARED ${CORE_SOURCES} ${CORE_HEADERS})
target_link_libraries(core ${EXIV2_LIBRARIES})
qt5_use_modules(core Core)

generate_export_header(core EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/exports/core_export.h)
turnOnCpp11(core)


##################################################################################

if(BUILD_TESTS)

	set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

	find_package(CppUTest REQUIRED)

        include_directories(${CPPUTEST_INCLUDE_DIRS})

	set(TESTS
                tests/algoTests.cpp
                tests/coreTests.cpp
		tests/RunAllTests.cpp
		${CORE_SOURCES}
		)

	add_executable(core_tests ${TESTS})
	qt5_use_modules(core_tests Core)
	set_target_properties(core_tests PROPERTIES COMPILE_FLAGS -DCORE_STATIC_DEFINE)
        target_link_libraries(core_tests ${CPPUTEST_LIBRARIES} ${EXIV2_LIBRARIES})

	turnOnCpp11(core_tests)
	
	add_test(core core_tests)

endif(BUILD_TESTS)
