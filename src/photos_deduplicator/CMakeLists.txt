
find_package(Corrosion REQUIRED)

# copy czkawka_core to build dir
# it is required for 2 reasons:
# 1. we do not want whole czkawka to be build (cargo compiles whole project no matter which subproject is being pointed)
# 2. some minor changes in .toml file are required
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/czkawka/czkawka_core DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/czkawka_core/Cargo.toml
    "[lib]
     crate-type=[\"staticlib\"]"
)

corrosion_import_crate(MANIFEST_PATH ${CMAKE_CURRENT_BINARY_DIR}/czkawka_core/Cargo.toml)

add_library(photos_deduplicator SHARED
    photos_deduplicator.cpp
)

target_link_libraries(photos_deduplicator
    PRIVATE
        czkawka_core
)
