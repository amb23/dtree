cmake_minimum_required(VERSION 3.20)

project(dtree LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 23)

find_package(GTest)

find_package(benchmark)

find_package(fmt)
if (fmt_FOUND)
    add_compile_definitions(SPDLOG_FMT_EXTERNAL)
endif()

find_package(spdlog REQUIRED)
# add_compile_definitions(SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG)

add_subdirectory(src)

if (GTest_FOUND)
    add_subdirectory(test)
endif()

if (benchmark_FOUND)
    add_subdirectory(benchmarks)
endif()
