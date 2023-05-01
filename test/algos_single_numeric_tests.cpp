
#include <gtest/gtest.h>

#include "dtree/algos/single_numeric.h"
#include "dtree/impurity_measures.h"
#include "dtree/labels.h"

TEST(test_algos_single_numeric, test_optimal_split)
{
    std::vector<double> feature { 0.6, 4.5, 0.2, 0.3, 7.8, 0.9 };
    dtree::labels labels { 0u, 1u, 0u, 0u, 1u, 0u };

    dtree::algos::optimal_split algo;

    auto [cost, splitting] = algo(feature, labels, dtree::gini_index);

    EXPECT_EQ(splitting, dtree::single_numeric_splitting(2.7));
    EXPECT_DOUBLE_EQ(cost, 0.0);
}

TEST(test_algos_single_numeric, test_median_split)
{
    std::vector<double> feature { 0.6, 4.5, 0.2, 0.3, 7.8, 0.9 };
    dtree::labels labels { 0u, 1u, 0u, 0u, 1u, 0u };

    dtree::algos::median_split algo;

    auto [cost, splitting]
        = algo(feature, labels, [](auto&& labels) { return labels[0] / 6.0; });

    EXPECT_EQ(splitting, dtree::single_numeric_splitting(0.75));
    EXPECT_DOUBLE_EQ(cost, 1.0 / 3.0);
}
