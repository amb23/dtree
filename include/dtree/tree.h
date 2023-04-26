#pragma once

#include <memory>
#include <variant>

#include "dtree/concepts.h"
#include "dtree/labels.h"

#include <iostream>

namespace dtree {

class Leaf;
class Branch;

using Node = std::variant<Leaf, Branch>;


class Leaf
{
public:

    Leaf(LabelDistribution labelDistribution)
        : mLabelDistribution{std::move(labelDistribution)}
    {
        std::cout << "Leaf(...)" << std::endl;
    }

    Leaf(const Leaf& leaf) : mLabelDistribution{leaf.mLabelDistribution}
    {
        std::cout << "Leaf(const Leaf&)" << std::endl;
    }

    Leaf(Leaf&& leaf) : mLabelDistribution{std::move(leaf.mLabelDistribution)}
    {
        std::cout << "Leaf(Leaf&&)" << std::endl;
    }

    ~Leaf() { std::cout << "~Leaf()" << std::endl; }

    const LabelDistribution& GetDistribution() const
    {
        return mLabelDistribution;
    }

private:

    LabelDistribution mLabelDistribution;
};


class Branch
{
public:

    Branch(FeatureID featureID, double cut, Node* lower, Node* upper)
        : mFeatureID{featureID}
        , mCut{cut}
        , mLower{lower}
        , mUpper{upper}
    {
        std::cout << "Branch(" << featureID << ", " << cut << ", ...)" << std::endl; 
    }
 
    Branch(Branch&& branch) noexcept
        : mFeatureID{branch.mFeatureID}
        , mCut{branch.mCut}
        , mLower{branch.mLower}
        , mUpper{branch.mUpper}
    {
        branch.mLower = nullptr;
        branch.mUpper = nullptr;

        std::cout << "Branch(Branch&&)" << std::endl;
    }

    ~Branch() {
        delete mLower;
        delete mUpper;
        std::cout << "~Branch()" << std::endl;
    }

    FeatureID GetFeatureID() const { return mFeatureID; }

    double GetCut() const { return mCut; }

    const Node& GetLower() const { return *mLower; }
    const Node& GetUpper() const { return *mUpper; }

private:

    FeatureID mFeatureID;
    double mCut;

    Node* mLower;
    Node* mUpper;
};


template <SamplePoint SamplePointT>
const LabelDistribution& GetDistribution(const SamplePointT& point, const Leaf& leaf)
{
    return leaf.GetDistribution();
}

template <SamplePoint SamplePointT>
const LabelDistribution& GetDistribution(const SamplePointT& point, const Branch& branch)
{
    double val = point[branch.GetFeatureID()];
    if (val <= branch.GetCut())
    {
        return std::visit([&point](const auto& node) -> decltype(auto) { return GetDistribution(point, node); }, branch.GetLower());
    }
    else
    {
        return std::visit([&point](const auto& node) -> decltype(auto) { return GetDistribution(point, node); }, branch.GetUpper());
    }
}

template <SamplePoint SamplePointT>
const LabelDistribution& GetDistribution(const SamplePointT& point, const Node& node)
{
    return std::visit([&point](const auto& v) -> decltype(auto) { return GetDistribution(point, v); }, node);
}


} // dtree
