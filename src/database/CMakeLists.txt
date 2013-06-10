
project(database)

find_package(OpenLibrary REQUIRED)
find_package(Boost 1.53.0 COMPONENTS crc)

include(${OPENLIBRARY_CMAKE_INCLUDES})

include_directories(${CMAKE_BINARY_DIR} ${OPENLIBRARY_INCLUDES} ${Boost_INCLUDE_DIRS} .)

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
target_link_libraries(database)

exportSymbols(database)
hideSymbols(database)
turnOnCpp11(database)
turnOnAllWarnings(${DATABASE_SOURCES})
