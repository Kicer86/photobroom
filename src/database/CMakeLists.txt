
project(database)

find_package(OpenLibrary REQUIRED)

include(${OPENLIBRARY_CMAKE_INCLUDES})

include_directories(${CMAKE_BINARY_DIR} ${OPENLIBRARY_INCLUDES} .)

set(DATABASE_SOURCES
    implementation/databasefactory.cpp 
    implementation/photosdatabase.cpp
    )

set(DATABASE_HEADERS 
    databasefactory.hpp
    iconfiguration.hpp
    idatabase.hpp
    implementation/photosdatabase.hpp
    )

source_group(database REGULAR_EXPRESSION .*database.* )

add_library(database STATIC ${DATABASE_SOURCES} ${DATABASE_HEADERS})
target_link_libraries(database)

exportSymbols(database)
hideSymbols(database)
turnOnCpp11(database)
turnOnAllWarnings(${DATABASE_SOURCES})
