#pragma once

#include <numeric>

#include "dtree/algos/types.h"
#include "dtree/concepts.h"
#include "dtree/labels.h"

namespace dtree::algos {

/// roh_split
///
/// roh = random optimal hyperplane; we take a random direction and
/// then calculate the optimal hyperplane in this direction.
class roh_split {
public:
    template <multi_feature_c feature_t, typename cost_fn_t>
    multi_dimensional_split_result operator()(
        const feature_t& feature, const labels& labels, cost_fn_t&& cost_fn) const
    {
        std::size_t n_features = feature.front().size();
        auto normal = generate_random_normal(n_features);

        using value_t = std::pair<double, labels::value_type>;
        std::vector<value_t> data;
        data.reserve(labels.size());
        for (std::size_t i = 0; i < labels.size(); ++i) {
            double val
                = std::inner_product(begin(normal), end(normal), begin(feature[i]), 0.0);
            data.emplace_back(val, labels[i]);
        }

        std::sort(begin(data), end(data));
        auto [cost, split] = find_optimal_split(data, labels, cost_fn);
        return multi_dimensional_split_result { cost, normal, split };
    }

private:
    std::vector<double> generate_random_normal(std::size_t n_features) const;
};

} // dtree::algos
