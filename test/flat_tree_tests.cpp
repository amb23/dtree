
#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>

#include "dtree/flat_tree.h"
#include "dtree/types.h"

struct test_splitting {
    template <typename T> bool operator()(const T& t) const { return t <= 0; }
};

using test_node = dtree::node<test_splitting>;

using test_flat_tree_t = dtree::flat_tree<test_node>;

TEST(test_flat_tree, test_apply)
{
    test_flat_tree_t test_flat_tree { 3 };
    // Tree of depth 3:
    // 0 -> 1, 2 // [1]
    // 1 -> 3, 4 // [2]
    // 2 -> 5, 6 // [2]
    // 3 -> 7, 8 // [3]
    // 4 -> 9, 10
    // 5 -> 11, 12
    // 6 -> 13, 14
    test_flat_tree[0] = test_node { 0u, test_splitting {} };
    test_flat_tree[1] = test_node { 0u, test_splitting {} };
    test_flat_tree[2] = dtree::leaf { { 0.25, 0.25, 0.5 } };
    test_flat_tree[3] = test_node { 1u, test_splitting {} };
    test_flat_tree[4] = test_node { 2u, test_splitting {} };
    // test_flat_tree[5] = unreachable
    // test_flat_tree[6] = unreachable
    test_flat_tree[7] = dtree::leaf { { 0.66, 0.12, 0.22 } };
    test_flat_tree[8] = dtree::leaf { { 0.1, 0.2, 0.7 } };
    test_flat_tree[9] = dtree::leaf { { 0.2, 0.3, 0.5 } };
    test_flat_tree[10] = dtree::leaf { { 0.3, 0.4, 0.3 } };
    // test_flat_tree[11] = test_node{0u, test_splitting{}};
    // test_flat_tree[12] = test_node{0u, test_splitting{}};
    // test_flat_tree[13] = test_node{0u, test_splitting{}};
    // test_flat_tree[14] = test_node{0u, test_splitting{}};

    EXPECT_THAT(test_flat_tree.apply(std::array { 1.0, 0.5, -0.9 }),
        ::testing::ElementsAreArray({ 0.25, 0.25, 0.5 }));

    EXPECT_THAT(test_flat_tree.apply(std::array { -0.1, 0.9, 0.8 }),
        ::testing::ElementsAreArray({ 0.1, 0.2, 0.7 }));

    EXPECT_THAT(test_flat_tree.apply(std::array { -0.1, -0.9, 0.8 }),
        ::testing::ElementsAreArray({ 0.66, 0.12, 0.22 }));
}

TEST(test_flat_tree, test_get_depth)
{
    EXPECT_EQ(test_flat_tree_t::get_depth(0), 0);
    EXPECT_EQ(test_flat_tree_t::get_depth(1), 1);
    EXPECT_EQ(test_flat_tree_t::get_depth(2), 1);
    EXPECT_EQ(test_flat_tree_t::get_depth(17), 4);
}

TEST(test_flat_tree, test_get_next)
{
    EXPECT_EQ(test_flat_tree_t::next(true, 0), 1);
    EXPECT_EQ(test_flat_tree_t::next(false, 0), 2);
    EXPECT_EQ(test_flat_tree_t::next(true, 1), 3);
    EXPECT_EQ(test_flat_tree_t::next(false, 1), 4);
    EXPECT_EQ(test_flat_tree_t::next(true, 2), 5);
    EXPECT_EQ(test_flat_tree_t::next(false, 2), 6);
    EXPECT_EQ(test_flat_tree_t::next(true, 27), 55);
    EXPECT_EQ(test_flat_tree_t::next(false, 27), 56);
}
