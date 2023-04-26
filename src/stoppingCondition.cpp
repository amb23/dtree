
#include <algorithm>
#include <sstream>

#include "dtree/stoppingCondition.h"


namespace dtree {


std::ostream& operator << (std::ostream& os, StoppedReason stoppedReason)
{
    switch (stoppedReason)
    {
    case StoppedReason::MaxDepth:
        os << "MaxDepth";
        break;
    case StoppedReason::MinSamples:
        os << "MinSamples";
        break;
    case StoppedReason::ProbabilityLimit:
        os << "ProbabilityLimit";
        break;
    default:
        // assert(false)
        break;
    }
    return os;
}


std::string ToString(StoppedReason stoppedReason)
{
    std::stringstream ss;
    ss << stoppedReason;
    return ss.str();
}


StoppingCondition::StoppingCondition(
    std::size_t maxDepth,
    std::size_t minSamples,
    double probabilityLimit)
    : mMaxDepth{maxDepth}
    , mMinSamples{minSamples}
    , mProbabilityLimit{std::clamp(probabilityLimit, 0.0, 1.0)}
{}

std::optional<StoppedReason> StoppingCondition::Check(
    std::size_t depth,
    const Labels& labels) const
{
    if (depth >= mMaxDepth) return StoppedReason::MaxDepth;
    if (labels.size() <= mMinSamples) return StoppedReason::MinSamples;

    const auto& dist = labels.CalculateDistribution();
    auto maxProb = std::max_element(begin(dist), end(dist));

    if (*maxProb >= mProbabilityLimit) return StoppedReason::ProbabilityLimit;

    return std::nullopt;
}

} // dtree
