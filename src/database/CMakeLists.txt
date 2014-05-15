
project(database CXX)

add_subdirectory(sql_backends)

find_package(OpenLibrary REQUIRED)
find_package(Boost 1.53.0 REQUIRED COMPONENTS system filesystem)
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/src ${CMAKE_BINARY_DIR}/exports)
include_directories(SYSTEM
                    ${OPENLIBRARY_INCLUDES}
                    ${Boost_INCLUDE_DIRS}
                    ${Qt5Core_INCLUDE_DIRS}
                    ${Qt5Gui_INCLUDE_DIRS})

set(DATABASE_SOURCES
    implementation/plugin_loader.cpp
    implementation/databasebuilder.cpp
    implementation/filter.cpp
    implementation/photo_iterator.cpp
    implementation/db_photo_info.cpp
    implementation/query_list.cpp
    )

set(DATABASE_HEADERS
    databasebuilder.hpp
    filter.hpp
    idatabase.hpp
    ifs.hpp
    photo_iterator.hpp
    query_list.hpp
    implementation/plugin_loader.hpp
    implementation/db_photo_info.hpp
    )

if(BUILD_TESTS)
    addTestTarget(database  SOURCES
                                tests/databaseTests.cpp
                                tests/photoIteratorTests.cpp
                            TEST_LIBRARY
                                GMOCK
       )
    target_link_libraries(database_test configuration core database ${Boost_LIBRARIES})
    qt5_use_modules(database_test Core Gui)

    turnOnCpp11(database_test)
endif(BUILD_TESTS)

source_group(files REGULAR_EXPRESSION .*database.* )

add_library(database ${DATABASE_SOURCES} ${DATABASE_HEADERS})
target_link_libraries(database configuration core ${Boost_LIBRARIES})
qt5_use_modules(database Core Gui)

generate_export_header(database EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/exports/database_export.h)
turnOnCpp11(database)
hideSymbols(database)

install(TARGETS database RUNTIME LIBRARY DESTINATION ${PATH_LIBS})
