#pragma once

#include "dtree/concepts.h"
#include "dtree/labels.h"
#include "dtree/splittings.h"

namespace dtree::algos {

template <typename splitting_t, typename feature_t, cost_fn_c cost_fn_t>
double calculate_cost(const splitting_t& splitting, const feature_t& feature,
    const labels& labels, cost_fn_t&& cost_fn)
{
    label_counts counts_above = labels.get_label_counts();
    label_counts counts_below(counts_above.size(), 0u);

    std::size_t total_above = labels.size();
    std::size_t total_below = 0u;

    for (std::size_t i = 0; i < labels.size(); ++i) {
        auto label = labels[i];

        if (splitting(feature[i])) {
            counts_above[label]--;
            counts_below[label]++;

            total_above--;
            total_below++;
        }
    }

    double total_count = labels.size();
    double cost = (total_above / total_count) * cost_fn(counts_above)
        + (total_below / total_count) * cost_fn(counts_below);

    return cost;
}

} // dtrees::algos
