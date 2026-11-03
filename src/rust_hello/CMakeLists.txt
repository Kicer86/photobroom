cmake_minimum_required(VERSION 3.30)  # Rust support requires CMake 3.30+

# Enable Rust language support (experimental in CMake 3.30+)
enable_language(Rust)

# Create a static library from Rust code
add_library(rust_hello STATIC lib.rs)

# Set Rust-specific properties
set_target_properties(rust_hello PROPERTIES
    Rust_CRATE_TYPE staticlib
)
