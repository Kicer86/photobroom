
find_package(Qt6 REQUIRED COMPONENTS Core Gui Sql)

set(BACKEND_SOURCES
        mysql_server.cpp
        backend.cpp
)

set(BACKEND_HEADERS
        mysql_server.hpp
        backend.hpp
)

include_directories(${CMAKE_SOURCE_DIR}/src)   #required by moc

if(STATIC_PLUGINS)
    add_definitions(-DSTATIC_PLUGINS)
endif()

qt6_wrap_cpp(BACKEND_WRAPPED_CPP backend.hpp )

add_library(database_mysql_backend MODULE ${BACKEND_SOURCES} ${BACKEND_HEADERS} ${BACKEND_WRAPPED_CPP})
target_link_libraries(database_mysql_backend PRIVATE core database sql_backend_base system Qt::Core Qt::Sql)
target_include_directories(database_mysql_backend
                            PUBLIC
                                ${CMAKE_CURRENT_BINARY_DIR}
                            PRIVATE
                                ${CMAKE_SOURCE_DIR}/src/database
)

hideSymbols(database_mysql_backend)
generate_export_header(database_mysql_backend)

install(TARGETS database_mysql_backend RUNTIME LIBRARY DESTINATION ${PATH_PLUGINS}/database)
