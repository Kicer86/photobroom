
add_executable(backends_benchmark)

target_sources(backends_benchmark
  PUBLIC FILE_SET CXX_MODULES FILES
    photo_fetch_benchmarks.cppm
    photo_fetch_benchmarks.cpp
)

target_sources(backends_benchmark
  PRIVATE
    main.cpp
)

target_link_libraries(backends_benchmark
    nanobench::nanobench

    database_memory_backend
    database_sqlite_backend
)
