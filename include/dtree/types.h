#pragma once

#include <cstdint>
#include <vector>

namespace dtree {

using feature_id = std::size_t;

template <typename splitting_t> struct node {
    bool operator==(const node&) const = default;
    feature_id feature_id_;
    splitting_t splitting;
};

// Tree types
template <typename node_type> class flat_tree;

// This helper class allows us to define the concrete tree type
// by the algorithm type that will be used to build it.
template <typename algo_t>
using flat_tree_t = flat_tree<node<typename algo_t::splitting_type>>;

using label_counts = std::vector<std::size_t>;

using label_distribution = std::vector<double>;

} // dtree
