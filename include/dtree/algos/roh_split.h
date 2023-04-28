#pragma once

#include "dtree/algos/types.h"
#include "dtree/labels.h"

namespace dtree::algos {

/// roh_split
///
/// roh = random optimal hyperplane; we take a random direction and
/// then calculate the optimal hyperplane in this direction.
class roh_split {
public:
    template <typename features_t, typename cost_fn_t>
    multi_dimensional_split_result operator()(
        const features_t& features,
        const labels& labels,
        cost_fn_t&& cost_fn) const
    {
        std::size_t n_features = features.size();
        auto normal = generate_random_normal(n_features);

        using value_t = std::pair<double, labels::value_type>;
        std::vector<value_t> data;
        data.reserve(labels.size());
        for (std::size_t i = 0; i < labels.size(); ++i) {
            double val = 0.0;
            for (std::size_t j = 0; const auto& [_, feature] : features) {
                val += feature[i] * normal[j++];
            }

            data.emplace_back(val, labels[i]);
        }

        std::sort(begin(data), end(data));
        return multi_dimensional_split_result { 0.0, normal, 0.0 };
    }

private:
    std::vector<double> generate_random_normal(std::size_t n_features) const;
};

} // dtree::algos
