
set(targets tree_builder impurity_measures splitter)

foreach(target IN LISTS targets)
    add_executable(${target}-${DTREE_VERSION}-bench
        ${target}_benchmarks.cpp
    )

    target_include_directories(${target}-${DTREE_VERSION}-bench
        PUBLIC
        ${CMAKE_SOURCE_DIR}/include
        ${benchmark_INCLUDE_DIRS}
    )

    target_link_libraries(${target}-${DTREE_VERSION}-bench
        PRIVATE
        benchmark::benchmark
        dtree
    )
endforeach()
