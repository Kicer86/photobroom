
find_package(Qt6 REQUIRED COMPONENTS Core Gui Sql)

set(BACKEND_SOURCES backend.cpp)
set(BACKEND_HEADERS backend.hpp)

if(STATIC_PLUGINS)
    add_definitions(-DSTATIC_PLUGINS)
endif()

include_directories(${CMAKE_SOURCE_DIR}/src)   #required by moc

qt6_wrap_cpp(BACKEND_MOCS ${BACKEND_HEADERS})

add_library(database_sqlite_backend MODULE ${BACKEND_SOURCES} ${BACKEND_HEADERS} ${BACKEND_MOCS})
target_link_libraries(database_sqlite_backend PRIVATE core database sql_backend_base Qt::Core Qt::Sql)
target_include_directories(database_sqlite_backend
                            PUBLIC
                                ${CMAKE_CURRENT_BINARY_DIR}
                            PRIVATE
                                ${CMAKE_SOURCE_DIR}/src/database
)

hideSymbols(database_sqlite_backend)
generate_export_header(database_sqlite_backend)

install(TARGETS database_sqlite_backend RUNTIME LIBRARY DESTINATION ${PATH_PLUGINS}/database)
