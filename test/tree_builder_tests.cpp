
#include <algorithm>
#include <valarray>

#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>

#include "dtree/algos/optimal_split.h"
#include "dtree/algos/roh_split.h"
#include "dtree/flat_tree.h"
#include "dtree/impurity_measures.h"
#include "dtree/splitter.h"
#include "dtree/treePrinter.h"
#include "dtree/tree_builder.h"

#include <iostream>

namespace {
using feature_set = std::unordered_map<std::size_t, std::vector<double>>;

feature_set test_features
    = { { 0, { 1.0, 0.5, 1.5, 0.5, 1.2, 0.9, 0.4, 1.8, 2.1, 7.2, 0.5, 0.4 } },
          { 1, { 0.1, 0.4, 0.2, 0.1, 0.4, 0.5, 0.8, 0.9, 0.7, 1.2, 0.8, 0.1 } },
          { 2, { 8.8, 1.3, 5.6, 0.4, 6.7, 7.8, 0.2, 0.7, 0.9, 1.0, 5.0, 9.9 } } };

dtree::labels test_labels { 0u, 1u, 1u, 0u, 1u, 1u, 1u, 1u, 1u, 0u, 0u, 1u };

using multi_feature_set
    = std::unordered_map<std::size_t, std::vector<std::valarray<double>>>;

multi_feature_set test_multi_features
    = { { 0,
            { { 1.0, 1.0 }, { 0.5, 0.6 }, { 0.7, 0.3 }, { -0.6, -0.7 }, { -0.6, 0.8 },
                { -1.0, -5.6 }, { 3.2, -6.7 } } },
          { 1,
              { { -9.0, 1.0 }, { 8.1, -6.6 }, { 4.7, -0.3 }, { 2.6, 5.7 },
                  { -1.6, -9.8 }, { -4.0, -5.6 }, { 6.6, 2.3 } } } };

dtree::labels test_labels2 { 0u, 1u, 0u, 1u, 1u, 1u, 0u };

}

TEST(tree_builder_tests, test_build)
{
    using namespace dtree;

    stop_condition stop_condition { 2 };

    using splitter_t = one_dimensional_splitter<algos::optimal_split>;
    tree_builder builder { tree_builder_config { false, 0u }, splitter_t {},
        gini_index };

    auto tree = builder.build(test_features, test_labels, stop_condition);

    // std::array<double, 3> sample{0.01, 0.02, 0.03};

    // auto dist = get_distribution(sample, *tree);
    // std::cout << "[0]: " << dist[0] << "  [1]: " << (dist.size() > 1 ? dist[1] : 0.0)
    // << std::endl;

    TreePrinter::Print(std::cout, tree);
}

TEST(tree_builder_tests, test_build_with_user_defined_cost)
{
    using namespace dtree;

    stop_condition stop_condition { 2 };

    using splitter_t = one_dimensional_splitter<algos::optimal_split>;
    tree_builder builder { tree_builder_config { false, 0u }, splitter_t {},
        [](const auto& counts) {
            double total = std::accumulate(begin(counts), end(counts), 0);
            return 1 - counts[0] / total;
        } };

    auto tree = builder.build(test_features, test_labels, stop_condition);

    // std::array<double, 3> sample{0.01, 0.02, 0.03};

    // auto dist = get_distribution(sample, *tree);
    // std::cout << "[0]: " << dist[0] << "  [1]: " << (dist.size() > 1 ? dist[1] : 0.0)
    // << std::endl;

    TreePrinter::Print(std::cout, tree);
}

TEST(tree_builder_tests, test_build_with_multi_features)
{
    using namespace dtree;

    stop_condition stop_condition { 2 };

    using splitter_t = multi_dimensional_splitter<algos::roh_split>;
    tree_builder builder { tree_builder_config { false, 0u }, splitter_t {},
        gini_index };

    auto tree = builder.build(test_multi_features, test_labels2, stop_condition);
    std::cout << "TREE built..." << std::endl;
    TreePrinter::Print(std::cout, tree);
}
