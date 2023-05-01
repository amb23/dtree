include(GoogleTest)

add_executable(dtreeTests
    algos_single_numeric_tests.cpp
    flat_tree_tests.cpp
    impurity_measures_tests.cpp
    serialization_tests.cpp
    splittings_tests.cpp
    tree_builder_tests.cpp
)

gtest_add_tests(TARGET dtreeTests)

target_include_directories(dtreeTests
    PRIVATE
    ${GTest_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(dtreeTests
    dtree
    GTest::gtest GTest::gtest_main GTest::gmock
)
