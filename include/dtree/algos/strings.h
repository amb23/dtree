#pragma once

#include <ranges>
#include <unordered_set>

#include "dtree/concepts.h"
#include "dtree/labels.h"
#include "dtree/splittings.h"

namespace dtree::algos {

template <typename base_algo_t> class string_length_split {
public:
    using splitting_type = string_length_splitting;

    template <string_feature_c feature_t, cost_fn_c cost_fn_t>
    std::pair<double, splitting_type> operator()(
        const feature_t& feature, const labels& labels, cost_fn_t&& cost_fn) const
    {
        auto [cost, splitting] = m_base_algo(
            feature | std::views::transform([](auto&& s) { return s.size(); }), labels,
            cost_fn);

        return { cost,
            splitting_type {
                static_cast<typename std::string::size_type>(splitting.split) } };
    }

private:
    base_algo_t m_base_algo;
};

class substring_split {
public:
    using splitting_type = has_substring_splitting;

    // TODO - this is incredibly inefficient
    template <string_feature_c feature_t, cost_fn_c cost_fn_t>
    std::pair<double, splitting_type> operator()(
        const feature_t& feature, const labels& labels, cost_fn_t&& cost_fn) const
    {
        double best_cost = std::numeric_limits<double>::max();
        splitting_type best_splitting;
        for (const auto& sstring : m_corpus) {
            splitting_type splitting { sstring };
            if (double cost = calculate_cost(splitting, feature, labels, cost_fn);
                cost < best_cost) {
                best_cost = cost;
                best_splitting = splitting;
            }
        }
        return { best_cost, best_splitting };
    }

private:
    const std::unordered_set<std::string>& m_corpus;
};

} // dtree::algos
