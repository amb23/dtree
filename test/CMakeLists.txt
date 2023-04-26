include(GoogleTest)

add_executable(dtreeTests
    impurityMeasureTests.cpp
    treeBuilderTests.cpp
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
