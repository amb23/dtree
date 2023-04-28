#pragma once

#include <algorithm>
#include <vector>

#include "dtree/algos/cost_utils.h"
#include "dtree/algos/types.h"
#include "dtree/labels.h"

namespace dtree::algos {

class optimal_split {
public:
    template <typename feature_t, typename cost_fn_t>
    one_dimensional_split_result operator()(
        const feature_t& feature, const labels& labels, cost_fn_t&& cost_fn) const
    {
        using value_t = std::pair<double, labels::label_t>;
        std::vector<value_t> data;
        data.reserve(labels.size());

        for (std::size_t i = 0; i < labels.size(); ++i) {
            data.emplace_back(feature[i], labels[i]);
        }

        std::sort(begin(data), end(data));
        return find_optimal_split(data, labels, cost_fn);
    }
};

} // dtree::algos
