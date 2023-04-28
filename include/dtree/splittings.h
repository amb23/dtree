#pragma once

#include <numeric>

#include "dtree/concepts.h"
#include "dtree/features.h"
#include "dtree/types.h"

namespace dtree {

struct one_dimensional_split {

    bool operator==(const one_dimensional_split&) const = default;

    feature_id feature_id_;
    double split;
};

struct multi_dimensional_split {

    bool operator==(const multi_dimensional_split&) const = default;

    feature_id feature_id_;
    std::vector<double> normal;
    double split;
};

template <sample_point sample_point_t>
bool is_lower(const sample_point_t& sample_point, const one_dimensional_split& split)
{
    return sample_point[split.feature_id_] <= split.split;
}

// FIXME - get a multi_sample_point_t
template <typename sample_point_t>
bool is_lower(const sample_point_t& sample_point, const multi_dimensional_split& split)
{
    using namespace std;
    const auto& feature_vals = sample_point[split.feature_id_];
    double norm_v = inner_product(
        begin(feature_vals), end(feature_vals), begin(split.normal), 0.0);

    return norm_v <= split.split;
}

} // dtree
