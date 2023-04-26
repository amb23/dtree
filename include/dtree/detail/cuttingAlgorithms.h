#pragma once

#include <algorithm>
#include <random>
#include <numeric>

#include <spdlog/spdlog.h>

#include "dtree/concepts.h"
#include "dtree/labels.h"


namespace dtree::detail {

// TODO - add in higher dimensional cuts

struct CutResult
{
    double mCost;
    double mCut;
    FeatureID mFeatureID;
};


// 

template <typename Cutter>
class OneDimensionalCut
{
public:

    template <typename... Args>
    OneDimensionalCut(Args&&... args) : mCutter{std::forward<Args>(args)...} {}

    template <typename Features, typename CostFn>
    CutResult Apply(const Features& features, const Labels& labels, CostFn&& costFn) const
    {
        double bestCost = std::numeric_limits<double>::max();
        double bestCut = 0.0;
        FeatureID bestFeatureID = std::numeric_limits<FeatureID>::max();

        for (const auto& [featureID, feature] : features)
        {
            auto [cost, cut] = mCutter(feature, labels, costFn);
            SPDLOG_DEBUG("Calculated cut {} for feature {} with cost {}", cut, featureID, cost);
            if (cost < bestCost)
            {
                bestCost = cost;
                bestCut = cut;
                bestFeatureID = featureID;
            }
        }

        return {bestCost, bestCut, bestFeatureID};
    }

private:

    Cutter mCutter;
};


template <typename Feature, typename CostFn>
double CalculateCost(double cut, const Feature& feature, const Labels& labels, CostFn& costFn)
{
    LabelCounts countsAbove = labels.GetLabelCounts();
    LabelCounts countsBelow(countsAbove.size(), 0u);

    std::size_t totalAbove = labels.size();
    std::size_t totalBelow = 0u;

    for (std::size_t i = 0; i < labels.size(); ++i)
    {
        auto label = labels[i];

        if (feature[i] <= cut)
        {
            countsAbove[label]--;
            countsBelow[label]++;

            totalAbove--;
            totalBelow++;
        }
    }

    double totalCount = labels.size();
    double cost = (totalAbove/totalCount) * costFn(countsAbove) +
        (totalBelow/totalCount) * costFn(countsBelow);

    return cost;

}

struct SingleFeatureCutResult
{
    double mCost;
    double mCut;
};

// Implementations
class OptimalCutter
{
public:

    template <typename Feature, typename CostFn>
    SingleFeatureCutResult  operator () (const Feature& feature, const Labels& labels, CostFn&& costFn) const
    {
        using value_t = std::pair<double, Labels::label_t>;
        std::vector<value_t> data;
        data.reserve(labels.size());

        for (std::size_t i = 0; i < labels.size(); ++i)
        {
            data.emplace_back(feature[i], labels[i]);
        }

        std::sort(begin(data), end(data));

        LabelCounts countsAbove = labels.GetLabelCounts();
        LabelCounts countsBelow(countsAbove.size(), 0u);

        std::size_t totalBelow = 0u, totalAbove = labels.size();

        double totalCount = labels.size();
        double currentCut = std::numeric_limits<double>::min();
        double bestCost = costFn(countsAbove);
    
        for (std::size_t i = 0; i < labels.size() - 1; ++i)
        {
            auto [z1, label] = data[i];

            countsBelow[label]++;
            countsAbove[label]--;

            totalBelow++;
            totalAbove--;

            double z2 = std::get<0>(data[i + 1]);
            if (z1 == z2) continue;

            double cost = (totalBelow/totalCount) * costFn(countsBelow) +
                (totalAbove/totalCount) * costFn(countsAbove);

            if (cost < bestCost)
            {
                bestCost = cost;
                currentCut = 0.5 * (z2 + z1);
            }
        }

        return {bestCost, currentCut};
    }
};


class RandomCutter
{
public:

    template <typename Feature, typename CostFn>
    SingleFeatureCutResult  operator () (const Feature& feature, const Labels& labels, CostFn&& costFn) const
    {
        // TODO - maybe something a little nicer
        auto loc = rand() % labels.size();

        double cut = feature[loc];

        return {CalculateCost(cut, feature, labels, costFn), cut};
    }
};


class MedianCutter
{
public:

    template <typename Feature, typename CostFn>
    SingleFeatureCutResult  operator () (const Feature& feature, const Labels& labels, CostFn&& costFn) const
    {
        std::vector<double> data(begin(feature), end(feature));
        std::sort(begin(data), end(data));

        std::size_t n = labels.size();
        double cut = n % 2 == 1
            ? feature[n/2]
            : 0.5 * (feature[n/2] + feature[1 + n/2]);

        return {CalculateCost(cut, feature, labels, costFn), cut};
    }
};

class AverageCutter
{
public:

    template <typename Feature, typename CostFn>
    SingleFeatureCutResult  operator () (const Feature& feature, const Labels& labels, CostFn&& costFn) const
    {
        double sum = std::accumulate(begin(feature), end(feature), 0);
        double cut = sum / labels.size();

        return {CalculateCost(cut, feature, labels, costFn), cut};
    }
};


} // dtree
