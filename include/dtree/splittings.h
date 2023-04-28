#pragma once

#include <numeric>

#include "dtree/concepts.h"
#include "dtree/features.h"
#include "dtree/types.h"

namespace dtree {

struct one_dimensional_split {
    feature_id feature_id_;
    double split;
};

struct multi_dimensional_split {
    std::vector<feature_id> feature_ids;
    std::vector<double> normal;
    double split;
};

template <sample_point sample_point_t>
bool is_lower(const sample_point_t& sample_point, const one_dimensional_split& split)
{
    return sample_point[split.feature_id_] <= split.split;
}

template <sample_point sample_point_t>
bool is_lower(const sample_point_t& sample_point, const multi_dimensional_split& split)
{
    sub_range_view sub_sample(split.feature_ids, sample_point);
    double norm_v = std::inner_product(
        begin(sub_sample), end(sub_sample), begin(split.normal), 0.0);

    return norm_v <= split.split;
}

} // dtree
