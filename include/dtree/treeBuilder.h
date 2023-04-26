#pragma once

#include <future>
#include <unordered_map>
#include <vector>

#include <spdlog/spdlog.h>

#include "dtree/helpers.h"
#include "dtree/labels.h"
#include "dtree/stoppingCondition.h"
#include "dtree/tree.h"


namespace dtree {

struct TreeBuilderConfig
{
    bool mAsync;
    std::size_t mAsyncSampleMin;
};


template <typename Cutter, typename CostFn>
class TreeBuilder
{
public:

    TreeBuilder(
        const TreeBuilderConfig& config,
        Cutter cutter,
        CostFn costFn)
        : mConfig{config}
        , mCutter{std::move(cutter)}
        , mCostFn{std::move(costFn)}
    {}

    template <typename FeatureSet>
    Node* Build(
        const FeatureSet& features,
        const Labels& labels,
        const StoppingCondition& stoppingCond) const
    {
        return Build(0u, features, labels, stoppingCond);
    }

private:

    template <typename FeatureSet>
    Node* Build(
        std::size_t depth,
        const FeatureSet& features,
        const Labels& labels,
        const StoppingCondition& stoppingCond) const
    {
        if (auto reason = stoppingCond.Check(depth, labels))
        {
            SPDLOG_DEBUG("Stopping building at depth {} as {}", depth, ToString(*reason));
            return new Node{Leaf{labels.CalculateDistribution()}};
        }

        auto [cost, cut, featureID] = mCutter.Apply(features, labels, mCostFn);
        SPDLOG_DEBUG("Found optimal cut {} for feature {} with cost {}", cut, featureID, cost);
    
        auto [lowerNode, upperNode] = BuildSubTrees(
            depth,
            features,
            labels,
            stoppingCond,
            featureID,
            cut);

        return new Node{Branch{featureID, cut, lowerNode, upperNode}};
    }

    template <typename FeatureSet>
    std::pair<Node*, Node*> BuildSubTrees(
        std::size_t currentDepth,
        const FeatureSet& features,
        const Labels& labels,
        const StoppingCondition& stoppingCond,
        FeatureID featureID,
        double cut) const
    {
        if (mConfig.mAsync && labels.size() > mConfig.mAsyncSampleMin)
        {
            return BuildSubTreesAsync(currentDepth, features, labels, stoppingCond, featureID, cut);
        }
        else
        {
            return BuildSubTreesSync(currentDepth, features, labels, stoppingCond, featureID, cut);
        }
    }
    
    template <typename FeatureSet>
    std::pair<Node*, Node*> BuildSubTreesAsync(
        std::size_t currentDepth,
        const FeatureSet& features,
        const Labels& labels,
        const StoppingCondition& stoppingCond,
        FeatureID featureID,
        double cut) const
    {
        auto lowerFuture = std::async(
            [this, &features, &labels, &stoppingCond, featureID, cut, depth=currentDepth + 1]()
            {
                return BuildSubTree(true, depth, features, labels, stoppingCond, featureID, cut);
            }
        );

        auto upperFuture = std::async(
            [this, &features, &labels, &stoppingCond, featureID, cut, depth=currentDepth + 1]()
            {
                return BuildSubTree(false, depth, features, labels, stoppingCond, featureID, cut);
            }
        );

        auto lowerTree = lowerFuture.get();
        auto upperTree = upperFuture.get();

        return {lowerTree, upperTree};
    }

    template <typename FeatureSet>
    std::pair<Node*, Node*> BuildSubTreesSync(
        std::size_t currentDepth,
        const FeatureSet& features,
        const Labels& labels,
        const StoppingCondition& stoppingCond,
        FeatureID featureID,
        double cut) const
    {
        auto lowerTree = BuildSubTree(
            true,
            currentDepth + 1,
            features,
            labels,
            stoppingCond,
            featureID,
            cut);

        auto upperTree = BuildSubTree(false, currentDepth + 1, features, labels, stoppingCond, featureID, cut);

        return {lowerTree, upperTree};
    }


    template <typename FeatureSet>
    Node* BuildSubTree(
        bool lower,
        std::size_t depth,
        const FeatureSet& features,
        const Labels& labels,
        const StoppingCondition& stoppingCond,
        FeatureID parentID,
        double parentCut) const
    {
        // We firstly need to take out the corresponding terms
        // TODO - make a feature set and feature concept
        std::unordered_map<FeatureID, std::vector<double>> cutFeatures;
        Labels cutLabels;

        // TODO - this will need to look different for a multi-feature cut
        const auto& cuttingFeature = features.find(parentID)->second;

        for (std::size_t i = 0; i < labels.size(); ++i)
        {
            double val = cuttingFeature[i];

            if (val <= parentCut != lower) continue;

            cutLabels.push_back(labels[i]);

            for (const auto& [featureID, feature] : features)
            {
                cutFeatures[featureID].push_back(feature[i]);
            }
        }

        return Build(depth, cutFeatures, cutLabels, stoppingCond);
    }


    TreeBuilderConfig mConfig;

    Cutter mCutter;

    CostFn mCostFn;
};

} // dtree
