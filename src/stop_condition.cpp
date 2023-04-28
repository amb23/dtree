
#include <algorithm>
#include <sstream>

#include "dtree/stop_condition.h"

namespace dtree {

std::ostream& operator<<(std::ostream& os, stopped_reason stopped_reason)
{
    switch (stopped_reason) {
    case stopped_reason::MaxDepth:
        os << "MaxDepth";
        break;
    case stopped_reason::MinSamples:
        os << "MinSamples";
        break;
    case stopped_reason::ProbabilityLimit:
        os << "ProbabilityLimit";
        break;
    default:
        // assert(false)
        break;
    }
    return os;
}

std::string to_string(stopped_reason stopped_reason)
{
    std::stringstream ss;
    ss << stopped_reason;
    return ss.str();
}

stop_condition::stop_condition(
    std::size_t max_depth,
    std::size_t min_samples,
    double probability_limit)
    : m_max_depth { max_depth }
    , m_min_samples { min_samples }
    , m_probability_limit { std::clamp(probability_limit, 0.0, 1.0) }
{
}

std::optional<stopped_reason> stop_condition::Check(
    std::size_t depth,
    const labels& labels) const
{
    if (depth >= m_max_depth)
        return stopped_reason::MaxDepth;
    if (labels.size() <= m_min_samples)
        return stopped_reason::MinSamples;

    const auto& dist = labels.calculate_distribution();
    auto max_prob = std::max_element(begin(dist), end(dist));

    if (*max_prob >= m_probability_limit)
        return stopped_reason::ProbabilityLimit;

    return std::nullopt;
}

} // dtree
