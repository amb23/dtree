#pragma once

#include <numeric>
#include <ranges>

#include "dtree/algos/single_numeric.h"
#include "dtree/concepts.h"
#include "dtree/labels.h"

namespace dtree::algos {

// seed=0 means we generate a random seed in the call.
std::vector<double> generate_random_normal(
    std::size_t n_features, unsigned int seed = 0);

template <typename base_algo_t> class random_hyperplane_split {
public:
    using splitting_type = multi_numeric_splitting;

    template <typename... arg_ts>
    explicit random_hyperplane_split(unsigned int seed, arg_ts&&... args)
        : m_seed { seed }
        , m_base_algo { std::forward<arg_ts>(args)... }
    {
    }

    template <typename... arg_ts>
    explicit random_hyperplane_split(arg_ts&&... args)
        : m_seed { 0u }
        , m_base_algo { std::forward<arg_ts>(args)... }
    {
    }

    template <multi_numeric_feature_c feature_t, typename cost_fn_t>
    std::pair<double, splitting_type> operator()(
        const feature_t& feature, const labels& labels, cost_fn_t&& cost_fn) const
    {
        std::size_t n_features = feature.front().size();
        auto normal = generate_random_normal(n_features);

        auto [cost, splitting]
            = m_base_algo(feature | std::views::transform([&normal](auto&& x) {
                  return std::inner_product(begin(x), end(x), begin(normal), 0.0);
              }),
                labels, cost_fn);

        return { cost, multi_numeric_splitting { normal, splitting.split } };
    }

private:
    unsigned int m_seed;

    base_algo_t m_base_algo;
};

} // dtree::algos
