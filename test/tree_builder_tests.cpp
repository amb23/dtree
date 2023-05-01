
#include <algorithm>
#include <valarray>

#include <boost/archive/xml_oarchive.hpp>
#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>
#include <spdlog/spdlog.h>

#include "dtree/algos/mixed_algo.h"
#include "dtree/algos/multi_numeric.h"
#include "dtree/algos/single_numeric.h"
#include "dtree/algos/strings.h"
#include "dtree/flat_tree.h"
#include "dtree/impurity_measures.h"
#include "dtree/tree_builder.h"

// FIXME - make all this nicer
namespace dtree::tests {

void check_equal(const label_distribution& seen, const label_distribution& expected)
{
    ASSERT_EQ(seen.size(), expected.size());
    for (std::size_t i = 0; i < seen.size(); ++i)
        EXPECT_DOUBLE_EQ(seen[i], expected[i]);
}

void check_equal(const leaf& seen, const leaf& expected)
{
    check_equal(seen.distribution(), expected.distribution());
}

void check_equal(
    const single_numeric_splitting& seen, const single_numeric_splitting& expected)
{
    EXPECT_DOUBLE_EQ(seen.split, expected.split);
}

template <typename splitting_t>
void check_equal(const node<splitting_t>& seen, const node<splitting_t>& expected)
{
    EXPECT_EQ(seen.feature_id_, expected.feature_id_);
    check_equal(seen.splitting, expected.splitting);
}

template <std::size_t current_index, typename... Ts>
void check_equal(std::size_t index, const std::variant<Ts...>& seen,
    const std::variant<Ts...>& expected)
{
    if constexpr (current_index < sizeof...(Ts)) {
        if (index == current_index) {
            check_equal(
                std::get<current_index>(seen), std::get<current_index>(expected));
        }
        check_equal<current_index + 1>(index, seen, expected);
    }
}

template <typename... Ts>
void check_equal(const std::variant<Ts...>& seen, const std::variant<Ts...>& expected)
{
    ASSERT_EQ(seen.index(), expected.index());
    check_equal<0>(seen.index(), seen, expected);
}

template <typename T>
void check_equal(const flat_tree<T>& seen, const flat_tree<T>& expected)
{
    ASSERT_EQ(seen.data().size(), expected.data().size());
    for (std::size_t i = 0; i < seen.data().size(); ++i) {
        check_equal(seen[i], expected[i]);
    }
};

} // dtree:tests

TEST(tree_builder_tests, test_single_numeric_build)
{
    using namespace dtree;

    using feature_set = std::unordered_map<std::size_t, std::vector<double>>;

    feature_set test_features
        = { { 0, { 1.0, 0.5, 1.5, 0.5, 1.2, 0.9, 0.4, 1.8, 2.1, 7.2, 0.5, 0.4 } },
              { 1, { 0.1, 0.4, 0.2, 0.1, 0.4, 0.5, 0.8, 0.9, 0.7, 0.6, 0.8, 0.1 } },
              { 2, { 8.8, 1.3, 5.6, 0.4, 6.7, 7.8, 0.2, 0.7, 0.9, 1.0, 5.0, 9.9 } } };

    dtree::labels test_labels { 0u, 1u, 1u, 0u, 1u, 1u, 1u, 1u, 1u, 0u, 0u, 1u };

    tree_builder builder { tree_builder_config { false, 0u, 2u, 1u, 1.0 },
        algos::optimal_split {}, gini_index };

    auto tree = builder.build(test_features, test_labels);

    flat_tree_t<algos::optimal_split> expected_tree { 2 };
    using node_type = node<algos::optimal_split::splitting_type>;
    expected_tree[0] = node { 0u, single_numeric_splitting { 4.65 } };
    expected_tree[1] = node { 1u, single_numeric_splitting { 0.15 } };
    expected_tree[2] = leaf { { 1.0 } };
    expected_tree[3] = leaf { { 2.0 / 3.0, 1.0 / 3.0 } };
    expected_tree[4] = leaf { { 0.125, 0.875 } };

    tests::check_equal(tree, expected_tree);
}

TEST(tree_builder_tests, test_build_with_user_defined_cost)
{
    using namespace dtree;

    using feature_set = std::unordered_map<std::size_t, std::vector<double>>;

    feature_set test_features
        = { { 0, { 1.0, 0.5, 1.5, 0.5, 1.2, 0.9, 0.4, 1.8, 2.1, 7.2, 0.5, 0.4 } },
              { 1, { 0.1, 0.4, 0.2, 0.1, 0.4, 0.5, 0.8, 0.9, 0.7, 0.6, 0.8, 0.1 } },
              { 2, { 8.8, 1.3, 5.6, 1.4, 6.7, 7.8, 0.2, 0.7, 0.1, 1.0, 5.0, 9.9 } } };

    dtree::labels test_labels { 0u, 1u, 1u, 0u, 0u, 1u, 0u, 1u, 1u, 0u, 0u, 1u };

    tree_builder builder { tree_builder_config { false, 0u, 2u, 1u, 1.0 },
        algos::optimal_split {}, [](const auto& counts) {
            double total = std::accumulate(begin(counts), end(counts), 0.0);
            if (total == 0.0)
                return 0.0;
            auto max_c = std::max_element(begin(counts), end(counts));
            auto min_c = std::min_element(begin(counts), end(counts));
            return (static_cast<double>(*min_c) - static_cast<double>(*max_c)) / total;
        } };

    auto tree = builder.build(test_features, test_labels);

    flat_tree_t<algos::optimal_split> expected_tree { 2 };
    using node_type = node<algos::optimal_split::splitting_type>;
    expected_tree[0] = node { 0u, single_numeric_splitting { 1.35 } };
    expected_tree[1] = node { 2u, single_numeric_splitting { 7.25 } };
    expected_tree[2] = node { 0u, single_numeric_splitting { 4.65 } };
    expected_tree[3] = leaf { { 0.8, 0.2 } };
    expected_tree[4] = leaf { { 1.0 / 3.0, 2.0 / 3.0 } };
    expected_tree[5] = leaf { { 0.0, 1.0 } };
    expected_tree[6] = leaf { { 1.0 } };

    tests::check_equal(tree, expected_tree);
}

TEST(tree_builder_tests, test_build_with_multi_features)
{
    spdlog::set_level(spdlog::level::debug);
    using namespace dtree;
    using multi_feature_set
        = std::unordered_map<std::size_t, std::vector<std::valarray<double>>>;

    multi_feature_set test_multi_features
        = { { 0,
                { { 1.0, 1.0 }, { 0.5, 0.6 }, { 0.7, 0.3 }, { -0.6, -0.7 },
                    { -0.6, 0.8 }, { -1.0, -5.6 }, { 3.2, -6.7 } } },
              { 1,
                  { { -9.0, 1.0 }, { 8.1, -6.6 }, { 4.7, -0.3 }, { 2.6, 5.7 },
                      { -1.6, -9.8 }, { -4.0, -5.6 }, { 6.6, 2.3 } } } };

    labels test_labels { 0u, 1u, 0u, 1u, 1u, 1u, 0u };

    using algo_t = algos::random_hyperplane_split<algos::optimal_split>;
    tree_builder builder { tree_builder_config { false, 0u, 2u, 1u, 1.0 },
        algo_t { 133u }, gini_index };

    auto tree = builder.build(test_multi_features, test_labels);
    // flat_tree<algo_t> expected_tree{2};
    // expected_tree[0] = node{0u, single_numeric_splitting{1.35}};
    // expected_tree[1] = node{2u, single_numeric_splitting{7.25}};
    // expected_tree[2] = node{0u, single_numeric_splitting{4.65}};
    // expected_tree[3] = leaf{{0.8, 0.2}};
    // expected_tree[4] = leaf{{1.0/3.0, 2.0/3.0}};
    // expected_tree[5] = leaf{{0.0, 1.0}};
    // expected_tree[6] = leaf{{1.0}};
}

TEST(tree_builder_test, test_build_with_string_features)
{
    using namespace dtree;

    using feature_set_t = std::unordered_map<std::size_t, std::vector<std::string>>;
    feature_set_t feature_set { { 0,
                                    { "It", "was", "a", "bright", "day", "in", "April",
                                        "and", "the", "clocks" } },
        { 1,
            { "Stately,", "plump", "Buck", "Mulligan", "came", "from", "the",
                "stairhead,", "bearing", "a" } } };
    labels labels { 0u, 0u, 1u, 1u, 0u, 1u, 2u, 2u, 1u, 0u };

    using algo_t = algos::string_length_split<algos::optimal_split>;
    tree_builder builder { tree_builder_config { false, 0u, 2u, 1u, 1.0 }, algo_t {},
        gini_index };

    auto tree = builder.build(feature_set, labels);
}

TEST(tree_builder_tests, test_build_with_mixed_features)
{

    using mixed_feature_t
        = std::variant<std::vector<double>, std::vector<std::valarray<double>>>;

    using mixed_feature_set_t = std::unordered_map<std::size_t, mixed_feature_t>;

    mixed_feature_set_t test_mixed_feature_set
        = { { 0, std::vector<double> { 0.0, 0.4, 0.2, 0.7, 0.5, -0.7, 0.8 } },
              { 1,
                  std::vector<std::valarray<double>> { { 0.0, 0.5 }, { -0.1, 0.5 },
                      { 0.8, 0.4 }, { -0.9, 1.3 }, { 2.3, -0.6 }, { -0.6, -0.9 },
                      { 1.1, -0.1 } } } };

    dtree::labels test_labels3 { 0u, 1u, 0u, 1u, 0u, 0u, 1u };
    using namespace dtree;

    using algo_t = algos::mixed_algo<algos::optimal_split, algos::median_split,
        algos::random_hyperplane_split<algos::optimal_split>>;

    tree_builder builder { tree_builder_config { false, 0u, 2u, 1u, 1.0 }, algo_t {},
        gini_index };

    auto tree = builder.build(test_mixed_feature_set, test_labels3);
}

TEST(tree_builder_tests, test_build_with_user_defined_feature)
{
    using namespace dtree;
    struct dummy_feature {
        int val;
    };
    struct dummy_splitting {
        bool operator()(const dummy_feature& feature) const
        {
            return feature.val <= val;
        }
        int val;
    };
    // This is a common design, maybe should add a transformer
    struct {
        using splitting_type = dummy_splitting;
        std::pair<double, splitting_type> operator()(
            const std::vector<dummy_feature>& vals, const labels& l,
            double (*cost_fn)(const label_counts&)) const
        {
            algos::optimal_split impl;
            auto [cost, splitting]
                = impl(vals | std::views::transform([](auto&& x) { return x.val; }), l,
                    cost_fn);
            return { cost, splitting_type { static_cast<int>(splitting.split) } };
        }
    } dummy_algo;
    using feature_t = std::vector<dummy_feature>;
    using feature_set_t = std::unordered_map<feature_id, feature_t>;

    feature_set_t features { { 0, { { 1 }, { 7 }, { 3 }, { 14 }, { 12 }, { 3 } } } };

    labels test_labels { 0u, 0u, 0u, 0u, 1u, 1u };

    tree_builder builder { tree_builder_config { false, 0u, 2u, 1u, 1.0 }, dummy_algo,
        gini_index };

    auto tree = builder.build(features, test_labels);
}
