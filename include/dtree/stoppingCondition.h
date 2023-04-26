#pragma once

#include <optional>
#include <ostream>

#include "dtree/labels.h"


namespace dtree {

enum class StoppedReason
{
    MaxDepth,
    MinSamples,
    ProbabilityLimit
};


std::ostream& operator << (std::ostream&, StoppedReason);

std::string ToString(StoppedReason);


class StoppingCondition
{
public:

    StoppingCondition(
        std::size_t maxDepth,
        std::size_t minSamples = 1u,
        double probabilityLimit = 1.0);

    std::optional<StoppedReason> Check(std::size_t depth, const Labels&) const;

private:

    std::size_t mMaxDepth;
    std::size_t mMinSamples;
    double mProbabilityLimit;
};

} // dtree
