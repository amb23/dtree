
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <gtest/gtest.h>

#include "dtree/serialization.h"
#include "dtree/splittings.h"

template <typename oarchive_t, typename iarchive_t, typename T>
void check_serialization_for_archive(const T& t)
{
    std::stringstream ss;
    {
        oarchive_t oarchive { ss };
        oarchive << boost::serialization::make_nvp("data", t);
    }
    {
        iarchive_t iarchive { ss };
        T out;
        iarchive >> boost::serialization::make_nvp("data", out);

        EXPECT_EQ(out, t);
    }
}

template <typename T> void check_serialization(const T& t)
{
    using namespace boost::archive;
    check_serialization_for_archive<binary_oarchive, binary_iarchive>(t);
    check_serialization_for_archive<text_oarchive, text_iarchive>(t);
    check_serialization_for_archive<xml_oarchive, xml_iarchive>(t);
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

TEST(test_serialization, test_single_numeric_splitting)
{
    dtree::single_numeric_splitting splitting { 0.9 };
    check_serialization(splitting);
}

TEST(test_serialization, test_multi_numeric_splitting)
{
    dtree::multi_numeric_splitting splitting { { 0.8, 0.6 }, 0.9 };
    check_serialization(splitting);
}

TEST(test_serialization, test_has_substring_splitting)
{
    dtree::has_substring_splitting splitting {
        "sent all up her body a hardness, a hollowness, a strain"
    };
    check_serialization(splitting);
}

TEST(test_serialization, test_string_length_splitting)
{
    dtree::string_length_splitting splitting { 34 };
    check_serialization(splitting);
}

TEST(test_serialization, test_leaf)
{
    dtree::leaf l { dtree::label_distribution { 0.2, 0.15, 0.15, 0.5 } };
    check_serialization(l);
}

TEST(test_serialization, test_flat_tree_one_dimensional)
{
    using namespace dtree;
    using node_type = node<single_numeric_splitting>;
    flat_tree<node_type> t(4);
    t[0] = node_type { 23, { 0.67 } };
    t[10] = leaf { label_distribution { 0.2, 0.15, 0.15, 0.5 } };
    check_serialization(t);
}
