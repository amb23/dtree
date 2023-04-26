#pragma once

#include <future>
#include <unordered_map>
#include <vector>

#include <spdlog/spdlog.h>

#include "dtree/labels.h"
#include "dtree/stoppingCondition.h"
#include "dtree/tree.h"


namespace dtree {

template <typename FeatureSet, typename CostFn>
class OptimalTreeBuilder
{
public:

    OptimalTreeBuilder(
        FeatureSet features,
        Labels labels,
        std::size_t depth,
        CostFn costFn)
        : mFeatures{std::move(features)}
        , mLabels{std::move(labels)}
        , mDepth{depth}
        , mCostFn(costFn)
    {}

    Tree Build(const StoppingCondition& stoppingCondition) const
    {
        if (auto reason = stoppingCondition.Check(mDepth, mLabels))
        {
            SPDLOG_DEBUG("Stopping building at depth {} as {}", mDepth, 6);
            return std::make_unique<Node>(Leaf{mLabels.CalculateDistribution()});
        }

        auto [featureID, cut] = GetOptimalCut();

        auto f1 = BuildSubTree<true>(featureID, cut, stoppingCondition);
        auto f2 = BuildSubTree<false>(featureID, cut, stoppingCondition);


        return std::make_unique<Node>(Branch{featureID, cut, f1.get(), f2.get()});
    }


private:

    std::pair<FeatureID, double> GetOptimalCut() const
    {
        double bestCost = std::numeric_limits<double>::max();
        double bestCut = 0.0;
        FeatureID bestFeatureID = std::numeric_limits<FeatureID>::max();

        for (const auto& [featureID, feature] : mFeatures)
        {
            auto [cost, cut] = GetOptimalCut(feature);
            SPDLOG_DEBUG("Calculated cut {} for feature {} with cost {}", cut, featureID, cost);
            if (cost < bestCost)
            {
                bestCost = cost;
                bestCut = cut;
                bestFeatureID = featureID;
            }
        }

        return {bestFeatureID, bestCut};
    }

    template <typename Feature>
    std::pair<double, double> GetOptimalCut(const Feature& feature) const
    {
        // TODO - is this necessary
        static_assert(std::is_same_v<typename Feature::value_type, double>);

        using value_t = std::pair<double, Labels::label_t>;
        std::vector<value_t> data;
        data.reserve(mLabels.size());

        for (std::size_t i = 0; i < mLabels.size(); ++i)
        {
            data.emplace_back(feature[i], mLabels[i]);
        }

        std::sort(begin(data), end(data));

        LabelCounts countsAbove = mLabels.GetLabelCounts();
        LabelCounts countsBelow(countsAbove.size(), 0u);

        std::size_t totalBelow = 0u, totalAbove = mLabels.size();

        double totalCount = mLabels.size();
        double currentCut = std::numeric_limits<double>::min();
        double bestCost = mCostFn(countsAbove);
    
        for (std::size_t i = 0; i < mLabels.size() - 1; ++i)
        {
            auto [z1, label] = data[i];

            countsBelow[label]++;
            countsAbove[label]--;

            totalBelow++;
            totalAbove--;

            double z2 = std::get<0>(data[i + 1]);
            if (z1 == z2) continue;

            double cost = (totalBelow/totalCount) * mCostFn(countsBelow) +
                (totalAbove/totalCount) * mCostFn(countsAbove);

            if (cost < bestCost)
            {
                bestCost = cost;
                currentCut = 0.5 * (z2 + z1);
            }
        }

        return {bestCost, currentCut};
    }

    template <bool IsLower>
    std::future<Tree> BuildSubTree(
        FeatureID featureID,
        double cut,
        const StoppingCondition& stoppingCondition) const
    {
        using FeatureSet_ = std::unordered_map<FeatureID, std::vector<double>>; 
        FeatureSet_ cutFeatures;
        Labels cutLabels;

        const auto& cuttingFeature = mFeatures.find(featureID)->second;

        for (std::size_t i = 0; i < mLabels.size(); ++i)
        {
            double cutFeature = cuttingFeature[i];

            if (cutFeature <= cut != IsLower)
            {
                continue;
            }

            for (const auto& [featureID, feature] : mFeatures)
            {
                cutFeatures[featureID].push_back(feature[i]);
            }

            cutLabels.push_back(mLabels[i]);
        }

        // TODO - can create twisted trees by choosing other
        // builders here. Will need a factory func
        using BuilderT = OptimalTreeBuilder<FeatureSet_, CostFn>;
        auto builder = std::make_unique<BuilderT>(
            std::move(cutFeatures),
            std::move(cutLabels),
            mDepth + 1u,
            mCostFn);

        return std::async(
            [b=std::move(builder), &stoppingCondition]()
            {
                return b->Build(stoppingCondition);
            });
    }

    FeatureSet mFeatures;
    Labels mLabels;

    std::size_t mDepth;
    CostFn mCostFn;
};

} // dtree
