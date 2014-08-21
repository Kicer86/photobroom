
project(database CXX)

add_subdirectory(sql_backends)

find_package(OpenLibrary REQUIRED)
find_package(Qt5Core REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/src ${CMAKE_BINARY_DIR}/exports)
include_directories(SYSTEM
                    ${OPENLIBRARY_INCLUDE_DIRS}
                    ${Qt5Core_INCLUDE_DIRS})

set(DATABASE_SOURCES
    implementation/plugin_loader.cpp
    implementation/database_builder.cpp
    implementation/database_thread.cpp
    implementation/filter.cpp
    implementation/photo_iterator.cpp
    implementation/photo_info.cpp
    implementation/photo_info_manager.cpp
    implementation/query_list.cpp
    )

set(DATABASE_HEADERS
    database_builder.hpp
    filter.hpp
    idatabase.hpp
    iphoto_info.hpp
    iphoto_info_creator.hpp
    iphoto_info_manager.hpp
    ifs.hpp
    photo_iterator.hpp
    query_list.hpp
    implementation/database_thread.hpp
    implementation/photo_info.hpp
    implementation/photo_info_manager.hpp
    implementation/plugin_loader.hpp
    )

set(DATABASE_MOC idatabase.hpp)

qt5_wrap_cpp(DATABASE_MOCED ${DATABASE_MOC})

if(BUILD_TESTS)
    addTestTarget(database  SOURCES
                                tests/databaseTests.cpp
                                tests/photoIteratorTests.cpp
                            TEST_LIBRARY
                                GMOCK
       )
    target_link_libraries(database_test configuration core database)
    qt5_use_modules(database_test Core Gui)

    turnOnCpp11(database_test)
endif(BUILD_TESTS)

source_group(files REGULAR_EXPRESSION .*database.* )

add_library(database ${DATABASE_SOURCES} ${DATABASE_HEADERS} ${DATABASE_MOCED})
target_link_libraries(database configuration core)
qt5_use_modules(database Core Gui)

generate_export_header(database EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/exports/database_export.h)
turnOnCpp11(database)
hideSymbols(database)

install(TARGETS database DESTINATION ${PATH_LIBS})
