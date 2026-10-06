find_package(benchmark CONFIG REQUIRED)

add_executable(ctprintf_benchmarks formatting.cpp)

target_link_libraries(ctprintf_benchmarks PRIVATE ctprintf::ctprintf benchmark::benchmark)
