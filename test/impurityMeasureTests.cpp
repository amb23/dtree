
#include <gtest/gtest.h>

#include "dtree/impurityMeasures.h"


TEST(ImpurityMeasureTesrs, GiniTest)
{
    using namespace dtree;
    LabelCounts labelCounts{4, 6};

    EXPECT_DOUBLE_EQ(GiniIndex(labelCounts), 0.48);
}
