
add_subdirectory(backends)

find_package(OpenLibrary 2.0 REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/src ${CMAKE_BINARY_DIR}/exports)
include_directories(SYSTEM
                    ${OPENLIBRARY_INCLUDE_DIRS}
                    ${Qt5Core_INCLUDE_DIRS})

set(DATABASE_SOURCES
    implementation/database_builder.cpp
    implementation/database_thread.cpp
    implementation/filter.cpp
    #implementation/photo_iterator.cpp     deprecated
    implementation/photo_info.cpp
    implementation/photo_info_cache.cpp
    implementation/photo_info_storekeeper.cpp
    #implementation/query_list.cpp         deprecated

    database_tools/implementation/photos_analyzer.cpp
    database_tools/implementation/photo_info_updater.cpp
    database_tools/implementation/photo_duplicates_finder.cpp
    database_tools/implementation/synchronous_database.cpp
    )

set(DATABASE_HEADERS
    database_builder.hpp
    filter.hpp
    idatabase.hpp
    iphoto_info.hpp
    iphoto_info_manager.hpp
    ifs.hpp
    photo_iterator.hpp
    query_list.hpp
    project_info.hpp
    backends/photo_info.hpp
    implementation/database_thread.hpp
    implementation/photo_info_cache.hpp
    implementation/photo_info_storekeeper.hpp
    database_tools/photos_analyzer.hpp
    database_tools/implementation/photo_info_updater.hpp
    database_tools/implementation/photo_duplicates_finder.hpp
    database_tools/implementation/synchronous_database.hpp
    )

set(DATABASE_MOC 
	idatabase.hpp 

	database_tools/photos_analyzer.hpp)

qt5_wrap_cpp(DATABASE_MOCED ${DATABASE_MOC})

if(BUILD_TESTS)
    addTestTarget(database  SOURCES
                                tests/databaseTests.cpp
                                tests/photoIteratorTests.cpp
                            TEST_LIBRARY
                                GMOCK
       )
    target_link_libraries(database_test PRIVATE core database Qt5::Core Qt5::Gui)

    turnOnCpp11(database_test)
endif(BUILD_TESTS)

source_group(files REGULAR_EXPRESSION .*database.* )

add_library(database SHARED ${DATABASE_SOURCES} ${DATABASE_HEADERS} ${DATABASE_MOCED})
target_link_libraries(database PRIVATE configuration core Qt5::Core Qt5::Gui)

generate_export_header(database EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/exports/database_export.h)
turnOnCpp11(database)
hideSymbols(database)

install(TARGETS database RUNTIME DESTINATION ${PATH_LIBS}
		         LIBRARY DESTINATION ${PATH_LIBS})
