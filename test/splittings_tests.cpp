
#include <array>

#include <gtest/gtest.h>

#include "dtree/splittings.h"

struct single_feature_test_data {
    std::array<double, 4> sample;
    bool expected_is_lower;
};

class one_dimensional_test_fixture
    : public ::testing::TestWithParam<single_feature_test_data> { };

TEST_P(one_dimensional_test_fixture, test_splitting)
{
    using namespace dtree;

    one_dimensional_split split { 2u, 0.7 };

    auto [sample, expected_lower] = GetParam();
    EXPECT_EQ(is_lower(sample, split), expected_lower);
}

INSTANTIATE_TEST_CASE_P(test_one_dimensional_split, one_dimensional_test_fixture,
    ::testing::Values(single_feature_test_data { { 1.0, 0.2, 0.4, 0.5 }, true },
        single_feature_test_data { { 1.0, 0.2, 0.9, 0.5 }, false }));

struct multi_feature_test_data {
    std::vector<std::array<double, 2>> sample;
    bool expected_is_lower;
};

class multi_dimensional_test_fixture
    : public ::testing::TestWithParam<multi_feature_test_data> { };

TEST_P(multi_dimensional_test_fixture, test_splitting)
{
    using namespace dtree;
    multi_dimensional_split split { 1u, { 0.5, -1, 0 }, 1.0 };

    auto [sample, expected_lower] = GetParam();
    EXPECT_EQ(is_lower(sample, split), expected_lower);
}

INSTANTIATE_TEST_CASE_P(test_multi_dimensional_split, multi_dimensional_test_fixture,
    ::testing::Values(multi_feature_test_data { { { 1.0, 0.2 }, { 0.4, 0.5 } }, true },
        multi_feature_test_data { { { 1.0, 0.2 }, { 1.0, -0.6 } }, false },
        multi_feature_test_data { { { 1.0, -0.2 }, { -1.0, -1.0 } }, true }));
