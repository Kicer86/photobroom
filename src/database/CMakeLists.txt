
project(database)

find_package(OpenLibrary REQUIRED)

include(${OPENLIBRARY_CMAKE_INCLUDES})

include_directories(${CMAKE_BINARY_DIR} ${OPENLIBRARY_INCLUDES} .)

if(UNIX)
	add_definitions(-DOS_UNIX)
elseif(WIN32)
	add_definitions(-DOS_WIN)
endif(UNIX)

set(DATABASE_SOURCES
    implementation/databasebuilder.cpp
    implementation/entry.cpp
    implementation/photosdatabase.cpp
    )

set(DATABASE_HEADERS
    databasebuilder.hpp
    iconfiguration.hpp
    idatabase.hpp
	ifs.hpp
    photosdatabase.hpp
	implementation/entry.hpp
    )

source_group(files REGULAR_EXPRESSION .*database.* )

add_library(database STATIC ${DATABASE_SOURCES} ${DATABASE_HEADERS})
target_link_libraries(database)

exportSymbols(database)
hideSymbols(database)
turnOnCpp11(database)
turnOnAllWarnings(${DATABASE_SOURCES})
