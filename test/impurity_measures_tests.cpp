
#include <gtest/gtest.h>

#include "dtree/impurity_measures.h"

struct impurity_measures_test_data {
    double (*measure)(const dtree::label_counts&);
    dtree::label_counts label_counts;
    double expected_value;
};

class impurity_measures_fixture
    : public ::testing::TestWithParam<impurity_measures_test_data> { };

TEST_P(impurity_measures_fixture, test_invoke)
{
    const auto& [measure, label_counts, expected_value] = GetParam();
    EXPECT_DOUBLE_EQ(measure(label_counts), expected_value);
}

INSTANTIATE_TEST_CASE_P(test_impurity_measures, impurity_measures_fixture,
    ::testing::Values(impurity_measures_test_data { dtree::gini_index, { 4, 6 }, 0.48 },
        impurity_measures_test_data {
            dtree::entropy, { 25, 67 }, 0.84394912448050341 }));
