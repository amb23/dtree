
#include <algorithm>

#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>

#include "dtree/cutters.h"
#include "dtree/impurityMeasures.h"
#include "dtree/tree.h"
#include "dtree/treeBuilder.h"
#include "dtree/treePrinter.h"


#include <iostream>

std::uint8_t operator""_l (unsigned long long v) { return static_cast<std::uint8_t>(v); }

TEST(TreeBuilderTests, TestBuild)
{
    using namespace dtree;

    using FeatureSet = std::unordered_map<std::size_t, std::vector<double>>;

    FeatureSet features = {
        {0, {1.0, 0.5, 1.5, 0.5, 1.2, 0.9, 0.4, 1.8, 2.1, 7.2, 0.5, 0.4}},
        {1, {0.1, 0.4, 0.2, 0.1, 0.4, 0.5, 0.8, 0.9, 0.7, 1.2, 0.8, 0.1}},
        {2, {8.8, 1.3, 5.6, 0.4, 6.7, 7.8, 0.2, 0.7, 0.9, 1.0, 5.0, 9.9}}
    };

    Labels labels{0u, 1u, 1u, 0u, 1u, 1u, 1u, 1u, 1u, 0u, 0u, 1u};

    StoppingCondition stoppingCondition{2};

    TreeBuilder builder{TreeBuilderConfig{false, 0u}, OptimalCut{}, GiniIndex};

    auto tree = builder.Build(features, labels, stoppingCondition);

    std::array<double, 3> sample{0.01, 0.02, 0.03};

    auto dist = GetDistribution(sample, *tree);
    std::cout << "[0]: " << dist[0] << "  [1]: " << (dist.size() > 1 ? dist[1] : 0.0) << std::endl;

    TreePrinter::Print(std::cout, *tree);
}


