
add_executable(backends_benchmark
    main.cpp
    photo_fetch_benchmarks.cpp
)

target_link_libraries(backends_benchmark
    nanobench::nanobench

    database_memory_backend
    database_sqlite_backend
)
