
find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(Boost REQUIRED)

add_library(database_memory_backend
    memory_backend.cpp
    memory_backend.hpp
)

target_include_directories(database_memory_backend
    PUBLIC
        ${PROJECT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_BINARY_DIR}
)


target_link_libraries(database_memory_backend
    PUBLIC
        Qt::Core
        Boost::boost

        database

    PRIVATE
        core
)

generate_export_header(database_memory_backend)
