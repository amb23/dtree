#pragma once

#include <algorithm>
#include <vector>

#include "dtree/algos/cost_utils.h"
#include "dtree/algos/types.h"
#include "dtree/labels.h"

namespace dtree::algos {

class median_split {
public:
    template <typename feature_t, typename cost_fn_t>
    one_dimensional_split_result operator()(
        const feature_t& feature, const labels& labels, cost_fn_t&& cost_fn) const
    {
        std::vector<double> data(begin(feature), end(feature));
        std::sort(begin(data), end(data));

        std::size_t n = labels.size();
        double split
            = n % 2 == 1 ? feature[n / 2] : 0.5 * (feature[n / 2] + feature[1 + n / 2]);

        return { calculate_cost(split, feature, labels, cost_fn), split };
    }
};

} // dtree::algos
