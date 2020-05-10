
find_package(Qt5 REQUIRED COMPONENTS Core)

add_library(JsonBackend
                json_backend.cpp
                json_backend.hpp
)

target_include_directories(JsonBackend
                               PUBLIC
                                   ${PROJECT_SOURCE_DIR}/src
)


target_link_libraries(JsonBackend
                          PUBLIC
                              Qt5::Core

                          PRIVATE
                              core
                              database
)
