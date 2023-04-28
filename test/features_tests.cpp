
#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>

#include "dtree/features.h"

TEST(sub_range_tests, test_vector_view)
{
    using namespace dtree;

    std::vector<double> data = { 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0 };
    std::vector<std::size_t> indexes = { 0, 2, 4, 5, 8 };

    sub_range_view view(indexes, data);

    EXPECT_THAT(view, testing::ElementsAreArray({ 0.1, 0.3, 0.5, 0.6, 0.9 }));

    // Write over the 3rd element in the sub range
    view[2] = 1.1;

    EXPECT_THAT(data,
        testing::ElementsAreArray({ 0.1, 0.2, 0.3, 0.4, 1.1, 0.6, 0.7, 0.8, 0.9, 1.0 }));

    std::vector<double> copy(begin(view), end(view));

    EXPECT_THAT(view, testing::ElementsAreArray({ 0.1, 0.3, 1.1, 0.6, 0.9 }));
}
