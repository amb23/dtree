
#include <gtest/gtest.h>

#include "dtree/serialization.h"

template <typename T> void check_serialization(const T& t)
{
    using namespace dtree;

    std::stringstream ss;
    serialize(ss, t);

    auto out = deserialize<T>(ss);

    EXPECT_EQ(out, t);
}

TEST(test_serialization, test_int)
{
    int x = 8;
    check_serialization(x);
}

TEST(test_serialization, test_long)
{
    long x = 89;
    check_serialization(x);
}

TEST(test_serialization, test_float)
{
    float x = 3.14f;
    check_serialization(x);
}

TEST(test_serialization, test_double)
{
    double x = 670.5;
    check_serialization(x);
}

TEST(test_serialization, test_vector_empty)
{
    std::vector<int> v {};
    check_serialization(v);
}

TEST(test_serialization, test_vector_int)
{
    std::vector<int> v { 1, 4, 3, 7 };
    check_serialization(v);
}

TEST(test_serialization, test_vector_vector_int)
{
    std::vector<std::vector<int>> v { { 1, 4 }, { 4, 3, 7 }, { 99 }, {} };
    check_serialization(v);
}

TEST(test_serialization, test_varaint_invalid_state)
{
    std::variant<int, float, double> v;
    check_serialization(v);
}

TEST(test_serialization, test_variant_zero_index)
{
    std::variant<int, float, double> v = 1;
    check_serialization(v);
}

TEST(test_serialization, test_variant_middle_index)
{
    std::variant<int, float, double> v = 1.0f;
    check_serialization(v);
}

TEST(test_serialization, test_variant_last_index)
{
    std::variant<int, float, double> v = 1.0;
    check_serialization(v);
}

TEST(test_serialization, test_variant_with_vector)
{
    std::variant<int, std::vector<int>> v = std::vector<int> { 6 };
    check_serialization(v);
}

TEST(test_serialization, test_one_dimensional_split)
{
    dtree::one_dimensional_split s { 346, 0.786 };
    check_serialization(s);
}

TEST(test_serialization, test_multi_dimensional_split)
{
    dtree::multi_dimensional_split s { 10544, std::vector { 0.56, -0.89, 1.23 }, 0.66 };
    check_serialization(s);
}

TEST(test_serialization, test_leaf)
{
    dtree::leaf l { dtree::label_distribution { 0.2, 0.15, 0.15, 0.5 } };
    check_serialization(l);
}

TEST(test_serialization, test_flat_tree_one_dimensional)
{
    dtree::flat_tree<dtree::one_dimensional_split> t(12);
    t[0] = dtree::one_dimensional_split { 23, 0.67 };
    t[100] = dtree::leaf { dtree::label_distribution { 0.2, 0.15, 0.15, 0.5 } };
    check_serialization(t);
}

#include <fstream>

TEST(test_serialization, test_flat_tree_file)
{
    dtree::flat_tree<dtree::one_dimensional_split> t(12);
    t[0] = dtree::one_dimensional_split { 23, 0.67 };
    t[100] = dtree::leaf { dtree::label_distribution { 0.2, 0.15, 0.15, 0.5 } };
    {
        std::fstream f { "tmp.data", std::ios::out };
        dtree::serialize(f, t);
    }
    {
        std::fstream f { "tmp.data", std::ios::in };
        auto out = dtree::deserialize<dtree::flat_tree<dtree::one_dimensional_split>>(f);
        EXPECT_EQ(out, t);
    }
}
