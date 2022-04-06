
find_package(Corrosion REQUIRED)
find_program(CBINDGEN cbindgen REQUIRED)

corrosion_import_crate(MANIFEST_PATH czkawka_interface/Cargo.toml)

add_custom_command(
    OUTPUT czkawka_interface.h
    COMMAND ${CBINDGEN}
    ARGS --config cbindgen.toml
         --output ${CMAKE_CURRENT_BINARY_DIR}/czkawka_interface.h
    DEPENDS
        czkawka_interface/src/similar_images_c.rs

    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/czkawka_interface
)

add_library(photos_deduplicator SHARED
    photos_deduplicator.cpp
    implementation/similar_images.cpp
    implementation/similar_images.hpp
    ${CMAKE_CURRENT_BINARY_DIR}/czkawka_interface.h
)

target_include_directories(photos_deduplicator
    PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(photos_deduplicator
    PRIVATE
        czkawka_interface
)
