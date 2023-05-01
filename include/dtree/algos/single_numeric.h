#pragma once

#include <algorithm>
#include <vector>

#include "dtree/algos/cost_utils.h"
#include "dtree/concepts.h"
#include "dtree/labels.h"
#include "dtree/splittings.h"

namespace dtree::algos {

class optimal_split {
public:
    using splitting_type = single_numeric_splitting;

    template <single_numeric_feature_c feature_t, cost_fn_c cost_fn_t>
    std::pair<double, splitting_type> operator()(
        const feature_t& feature, const labels& labels, cost_fn_t&& cost_fn) const
    {
        using value_t = std::pair<double, labels::label_t>;
        std::vector<value_t> data;
        data.reserve(labels.size());

        for (std::size_t i = 0; i < labels.size(); ++i) {
            data.emplace_back(feature[i], labels[i]);
        }

        std::sort(begin(data), end(data));
        label_counts counts_above = labels.get_label_counts();
        label_counts counts_below(counts_above.size(), 0u);

        std::size_t total_below = 0u, total_above = labels.size();

        double total_count = labels.size();
        double current_split = std::numeric_limits<double>::min();
        double best_cost = cost_fn(counts_above);

        for (std::size_t i = 0; i < labels.size() - 1; ++i) {
            auto [z1, label] = data[i];

            counts_below[label]++;
            counts_above[label]--;

            total_below++;
            total_above--;

            double z2 = std::get<0>(data[i + 1]);
            if (z1 == z2)
                continue;

            double cost = (total_below / total_count) * cost_fn(counts_below)
                + (total_above / total_count) * cost_fn(counts_above);

            if (cost < best_cost) {
                best_cost = cost;
                current_split = 0.5 * (z2 + z1);
            }
        }

        return { best_cost, single_numeric_splitting { current_split } };
    }
};

class median_split {
public:
    using splitting_type = single_numeric_splitting;

    template <single_numeric_feature_c feature_t, cost_fn_c cost_fn_t>
    std::pair<double, splitting_type> operator()(
        const feature_t& feature, const labels& labels, cost_fn_t&& cost_fn) const
    {
        std::vector<double> data(begin(feature), end(feature));
        std::sort(begin(data), end(data));

        std::size_t n = labels.size();
        double split
            = n % 2 == 1 ? data[n / 2] : 0.5 * (data[n / 2] + data[(n / 2) - 1]);

        splitting_type splitting { split };
        return { calculate_cost(splitting, feature, labels, cost_fn), splitting };
    }
};

} // dtree::algos
