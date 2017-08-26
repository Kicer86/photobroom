
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Sql REQUIRED)
find_package(OpenLibrary REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/src/
                    ${CMAKE_SOURCE_DIR}/src/database/
                    ${CMAKE_BINARY_DIR}/exports)

include_directories(SYSTEM ${Qt5Core_INCLUDE_DIRS}
                           ${Qt5Gui_INCLUDE_DIRS}
                           ${Qt5Sql_INCLUDE_DIRS}
                           ${OPENLIBRARY_INCLUDE_DIRS})

set(BACKEND_SOURCES
        mysql_server.cpp
        backend.cpp
)

set(BACKEND_HEADERS
        mysql_server.hpp
        backend.hpp
)

qt5_wrap_cpp(BACKEND_WRAPPED_CPP backend.hpp )

add_library(database_mysql_backend MODULE ${BACKEND_SOURCES} ${BACKEND_HEADERS} ${BACKEND_WRAPPED_CPP})
target_link_libraries(database_mysql_backend PRIVATE core database sql_backend_base system Qt5::Core Qt5::Sql)
target_include_directories(database_mysql_backend PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

hideSymbols(database_mysql_backend)
generate_export_header(database_mysql_backend)

install(TARGETS database_mysql_backend RUNTIME LIBRARY DESTINATION ${PATH_PLUGINS}/database)
