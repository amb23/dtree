#pragma once

#include <optional>
#include <ostream>

#include "dtree/labels.h"

namespace dtree {

enum class stopped_reason { MaxDepth, MinSamples, ProbabilityLimit };

std::ostream& operator<<(std::ostream&, stopped_reason);

std::string to_string(stopped_reason);

class stop_condition {
public:
    stop_condition(std::size_t max_depth, std::size_t min_samples = 1u,
        double probability_limit = 1.0);

    std::size_t get_max_depth() const { return m_max_depth; }

    std::optional<stopped_reason> Check(std::size_t depth, const labels&) const;

private:
    std::size_t m_max_depth;
    std::size_t m_min_samples;
    double m_probability_limit;
};

} // dtree
