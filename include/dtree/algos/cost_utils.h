#pragma once

#include "dtree/algos/types.h"
#include "dtree/labels.h"

namespace dtree::algos {

template <typename feature_t, typename cost_fn_t>
double calculate_cost(
    double split, const feature_t& feature, const labels& labels, cost_fn_t&& cost_fn)
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
    double cost = (total_above / total_count) * cost_fn(counts_above)
        + (total_below / total_count) * cost_fn(counts_below);

    return cost;
}

// The marked labels must be sorted
template <typename marked_labels_t, typename cost_fn_t>
one_dimensional_split_result find_optimal_split(
    const marked_labels_t& marked_labels, const labels& labels, cost_fn_t&& cost_fn)
{
    label_counts counts_above = labels.get_label_counts();
    label_counts counts_below(counts_above.size(), 0u);

    std::size_t total_below = 0u, total_above = labels.size();

    double total_count = labels.size();
    double current_split = std::numeric_limits<double>::min();
    double best_cost = cost_fn(counts_above);

    for (std::size_t i = 0; i < labels.size() - 1; ++i) {
        auto [z1, label] = marked_labels[i];

        counts_below[label]++;
        counts_above[label]--;

        total_below++;
        total_above--;

        double z2 = std::get<0>(marked_labels[i + 1]);
        if (z1 == z2)
            continue;

        double cost = (total_below / total_count) * cost_fn(counts_below)
            + (total_above / total_count) * cost_fn(counts_above);

        if (cost < best_cost) {
            best_cost = cost;
            current_split = 0.5 * (z2 + z1);
        }
    }

    return { best_cost, current_split };
}

} // dtrees::algos
