
#include <gtest/gtest.h>

#include "dtree/impurity_measures.h"

TEST(impurity_measure_tests, gini_test)
{
    using namespace dtree;
    label_counts label_counts { 4, 6 };

    EXPECT_DOUBLE_EQ(gini_index(label_counts), 0.48);
}
