#pragma once

#include "dtree/labels.h"

namespace dtree::algos {

template <typename feature_t, typename cost_fn_t>
double calculate_cost(
    double split,
    const feature_t& feature,
    const labels& labels,
    cost_fn_t&& cost_fn)
{
    label_counts counts_above = labels.get_label_counts();
    label_counts counts_below(counts_above.size(), 0u);

    std::size_t total_above = labels.size();
    std::size_t total_below = 0u;

    for (std::size_t i = 0; i < labels.size(); ++i) {
        auto label = labels[i];

        if (feature[i] <= split) {
            counts_above[label]--;
            counts_below[label]++;

            total_above--;
            total_below++;
        }
    }

    double total_count = labels.size();
    double cost = (total_above / total_count) * cost_fn(counts_above) + (total_below / total_count) * cost_fn(counts_below);

    return cost;
}

} // dtrees::algos
