
project(database)

find_package(OpenLibrary REQUIRED)
find_package(Boost 1.53.0 COMPONENTS crc)

include(GenerateExportHeader)

include_directories(${CMAKE_BINARY_DIR}/exports ${OPENLIBRARY_INCLUDES} ${Boost_INCLUDE_DIRS} .)

set(DATABASE_SOURCES
    implementation/backend.cpp
    implementation/databasebuilder.cpp
    implementation/entry.cpp
    implementation/memorydatabase.cpp
    )

set(DATABASE_HEADERS
    databasebuilder.hpp
    iconfiguration.hpp
    idatabase.hpp
    ifs.hpp
    entry.hpp
    implementation/backend.hpp
    implementation/memorydatabase.hpp
    )

source_group(files REGULAR_EXPRESSION .*database.* )

add_library(database SHARED ${DATABASE_SOURCES} ${DATABASE_HEADERS})

generate_export_header(database EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/exports/database_export.h)
turnOnCpp11(database)


##################################################################################


if(BUILD_TESTS)

	set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

	find_package(CppUTest REQUIRED)

	include_directories(${CMAKE_SOURCE_DIR}/src
						${CMAKE_SOURCE_DIR}/src/database
						${CPPUTEST_INCLUDE_DIRS}
						${OPENLIBRARY_INCLUDES}
						${CMAKE_BINARY_DIR})

	set(TESTS
		tests/databaseTests.cpp
		tests/RunAllTests.cpp
		${DATABASE_SOURCES}
		)

	add_executable(database_tests ${TESTS})
	set_target_properties(database_tests PROPERTIES COMPILE_FLAGS -DDATABASE_STATIC_DEFINE)
	target_link_libraries(database_tests ${CPPUTEST_LIBRARIES})

	turnOnCpp11(database_tests)
	
	if(CMAKE_COMPILER_IS_GNUCXX)
            #addFlags(database_tests COMPILE_FLAGS -pthread)
            addFlags(database_tests LINK_FLAGS -pthread)
	endif(CMAKE_COMPILER_IS_GNUCXX)
	
	add_test(database database_tests)

endif(BUILD_TESTS)
