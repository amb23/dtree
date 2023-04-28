
set(targets tree_builder impurity_measures splitter)

foreach(target IN LISTS targets)
    add_executable(${target}-bench
        ${target}_benchmarks.cpp
    )

    target_include_directories(${target}-bench
        PUBLIC
        ${CMAKE_SOURCE_DIR}/include
        ${benchmark_INCLUDE_DIRS}
    )

    target_link_libraries(${target}-bench
        PRIVATE
        benchmark::benchmark
        dtree
    )
endforeach()
