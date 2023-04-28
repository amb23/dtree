#pragma once

#include <variant>

#include <spdlog/spdlog.h>

#include "dtree/labels.h"
#include "dtree/splittings.h"

namespace dtree {

template <typename algo_t>
class one_dimensional_splitter {
public:
    using split_t = one_dimensional_split;

    template <typename... args_t>
    one_dimensional_splitter(args_t&&... args)
        : m_algo { std::forward<args_t>(args)... }
    {
    }

    template <typename feature_set_t, typename cost_fn_t>
    split_t split(
        const feature_set_t& feature_set,
        const labels& labels,
        cost_fn_t&& cost_fn) const
    {
        double best_cost = std::numeric_limits<double>::max();
        double best_split = 0.0;
        feature_id best_feature_id = std::numeric_limits<feature_id>::max();

        for (const auto& [feature_id, feature] : feature_set) {
            auto [cost, split] = m_algo(feature, labels, cost_fn);
            SPDLOG_DEBUG("Calculated split {} for feature {} with cost {}", split, feature_id, cost);
            if (cost < best_cost) {
                best_cost = cost;
                best_split = split;
                best_feature_id = feature_id;
            }
        }

        SPDLOG_DEBUG("Best split found for feature {} with cost {}", best_feature_id, best_cost);

        return { best_feature_id, best_split };
    }

private:
    algo_t m_algo;
};

template <typename one_d_algo_t, typename multi_d_algo_t>
class multi_dimensional_splitter {
public:
    using split_t = std::variant<one_dimensional_split, multi_dimensional_split>;

    multi_dimensional_splitter()
        : m_one_d_algo {}
        , m_multi_d_algo {}
    {
    }

    multi_dimensional_splitter(one_d_algo_t one_d_algo, multi_d_algo_t multi_d_algo)
        : m_one_d_algo { std::move(one_d_algo) }
        , m_multi_d_algo { std::move(multi_d_algo) }
    {
    }

    template <typename feature_set_t, typename cost_fn_t>
    split_t split(
        const feature_set_t& feature_set,
        const labels& labels,
        cost_fn_t&& cost_fn) const;

private:
    one_d_algo_t m_one_d_algo;
    multi_d_algo_t m_multi_d_algo;
};

} // dtree
