
#include <array>

#include <gtest/gtest.h>

#include "dtree/splittings.h"

struct single_numeric_splitting_test_data {
    dtree::single_numeric_splitting splitting;
    double sample;
    bool expected_lower;
};

class single_numeric_splitting_fixture
    : public ::testing::TestWithParam<single_numeric_splitting_test_data> { };

TEST_P(single_numeric_splitting_fixture, test_splitting)
{
    auto [splitting, sample, expected_lower] = GetParam();
    EXPECT_EQ(splitting(sample), expected_lower);
}

INSTANTIATE_TEST_CASE_P(test_single_numeric_splitting, single_numeric_splitting_fixture,
    ::testing::Values(single_numeric_splitting_test_data { { 0.7 }, 0.6, true },
        single_numeric_splitting_test_data { { 0.7 }, 0.9, false },
        single_numeric_splitting_test_data { { 0.7 }, 0.7, true }));

struct multi_numeric_splitting_test_data {
    dtree::multi_numeric_splitting splitting;
    std::array<double, 3> sample;
    bool expected_lower;
};

class multi_numeric_splitting_fixture
    : public ::testing::TestWithParam<multi_numeric_splitting_test_data> { };

TEST_P(multi_numeric_splitting_fixture, test_splitting)
{
    auto [splitting, sample, expected_lower] = GetParam();
    EXPECT_EQ(splitting(sample), expected_lower);
}

INSTANTIATE_TEST_CASE_P(test_multi_numeric_splitting, multi_numeric_splitting_fixture,
    ::testing::Values(multi_numeric_splitting_test_data { { { 0.4, 0.5, 1.4 }, 0.7 },
                          { 0.6, -0.5, 0.8 }, false },
        multi_numeric_splitting_test_data {
            { { 0.1, 0.3, -0.4 }, -0.5 }, { 0.9, -2.0, -1.0 }, false }));

struct has_substring_splitting_test_data {
    dtree::has_substring_splitting splitting;
    std::string sample;
    bool expected_lower;
};

class has_substring_splitting_fixture
    : public ::testing::TestWithParam<has_substring_splitting_test_data> { };

TEST_P(has_substring_splitting_fixture, test_spliting)
{
    auto [splitting, sample, expected_lower] = GetParam();
    EXPECT_EQ(splitting(sample), expected_lower);
}

INSTANTIATE_TEST_CASE_P(test_has_substring_splitting, has_substring_splitting_fixture,
    ::testing::Values(
        has_substring_splitting_test_data { { "golden" }, "gold ended here", false },
        has_substring_splitting_test_data { { "fun t" }, "wedi gorffun traeth", true }));

struct splitting_variant_test_data {
    dtree::splitting_variant<dtree::single_numeric_splitting,
        dtree::multi_numeric_splitting>
        splitting;
    std::variant<double, std::array<double, 3>> sample;
    bool expected_lower;
};

class splitting_variant_fixture
    : public ::testing::TestWithParam<splitting_variant_test_data> { };

TEST_P(splitting_variant_fixture, test_spliting)
{
    auto [splitting, sample, expected_lower] = GetParam();
    EXPECT_EQ(splitting(sample), expected_lower);
}

INSTANTIATE_TEST_CASE_P(test_splitting_variant, splitting_variant_fixture,
    ::testing::Values(
        splitting_variant_test_data {
            dtree::single_numeric_splitting { 0.7 }, { 0.6 }, true },
        splitting_variant_test_data {
            dtree::multi_numeric_splitting { { 0.4, 0.5, 1.4 }, 0.7 },
            std::array { 0.6, -0.5, 0.8 }, false }));
