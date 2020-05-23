
find_package(Qt5 REQUIRED COMPONENTS Core)

add_library(MemoryBackend
                memory_backend.cpp
                memory_backend.hpp
)

target_include_directories(MemoryBackend
                               PUBLIC
                                   ${PROJECT_SOURCE_DIR}/src
)


target_link_libraries(MemoryBackend
                          PUBLIC
                              Qt5::Core

                          PRIVATE
                              core
                              database
)
