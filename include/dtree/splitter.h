#pragma once

#include <variant>

#include <spdlog/spdlog.h>

#include "dtree/concepts.h"
#include "dtree/labels.h"
#include "dtree/splittings.h"

namespace dtree {

template <typename algo_t> class one_dimensional_splitter {
public:
    using split_t = one_dimensional_split;

    template <typename... args_t>
    one_dimensional_splitter(args_t&&... args)
        : m_algo { std::forward<args_t>(args)... }
    {
    }

    template <feature_set_c feature_set_t, typename cost_fn_t>
    split_t split(const feature_set_t& feature_set, const labels& labels,
        cost_fn_t&& cost_fn) const
    {
        double best_cost = std::numeric_limits<double>::max();
        return split(best_cost, feature_set, labels, cost_fn);
    }

    template <feature_set_c feature_set_t, typename cost_fn_t>
    split_t split(double& best_cost, const feature_set_t& feature_set,
        const labels& labels, cost_fn_t&& cost_fn) const
    {
        double best_split = 0.0;
        feature_id best_feature_id = std::numeric_limits<feature_id>::max();

        for (const auto& [feature_id, feature] : feature_set) {
            auto [cost, split] = m_algo(feature, labels, cost_fn);
            SPDLOG_DEBUG("Calculated split {} for feature {} with cost {}", split,
                feature_id, cost);
            if (cost < best_cost) {
                best_cost = cost;
                best_split = split;
                best_feature_id = feature_id;
            }
        }

        SPDLOG_DEBUG(
            "Best split found for feature {} with cost {}", best_feature_id, best_cost);

        return { best_feature_id, best_split };
    }

private:
    algo_t m_algo;
};

template <typename multi_d_algo_t> class multi_dimensional_splitter {
public:
    using split_t = multi_dimensional_split;

    template <typename... args_t>
    multi_dimensional_splitter(args_t&&... args)
        : m_algo { std::forward<args_t>(args)... }
    {
    }

    template <multi_feature_set_c feature_set_t, typename cost_fn_t>
    split_t split(const feature_set_t& feature_set, const labels& labels,
        cost_fn_t&& cost_fn) const
    {
        double best_cost = std::numeric_limits<double>::max();
        return split(best_cost, feature_set, labels, cost_fn);
    }

    template <multi_feature_set_c multi_feature_set_t, typename cost_fn_t>
    split_t split(double& best_cost, const multi_feature_set_t& feature_set,
        const labels& labels, cost_fn_t&& cost_fn) const
    {
        double best_split;
        std::vector<double> best_normal;
        feature_id best_feature_id = std::numeric_limits<feature_id>::max();

        for (const auto& [feature_id, feature] : feature_set) {
            auto result = m_algo(feature, labels, cost_fn);
            SPDLOG_DEBUG(
                "Calculated split for feature {} with cost {}", feature_id, cost);
            if (result.cost < best_cost) {
                best_cost = result.cost;
                best_split = result.split;
                best_normal = std::move(result.normal);
                best_feature_id = feature_id;
            }
        }

        SPDLOG_DEBUG(
            "Best split found for feature {} with cost {}", best_feature_id, best_cost);

        return { best_feature_id, best_normal, best_split };
    }

private:
    multi_d_algo_t m_algo;
};

/*
template <typename one_d_algo_t, typename multi_d_algo_t>
class mixed_feature_splitter
    : public one_dimensional_splitter<one_d_algo_t>
    , public multi_dimensional_splitter<multi_d_algo_t> {
    using one_d_base_t = one_dimensional_splitter<one_d_algo_t>;
    using multi_d_base_t = ulti_dimensional_splitter<multi_d_algo_t>;

    using one_d_split_t = one_d_base_t::split_t;
    using multi_d_split_t = multi_d_base_t::split_t;
public:
    using split_t = std::variant<one_d_split_t, multi_d_split_t>;

    using one_d_base_t::split;
    using multi_d_base_t::split;

    mixed_feature_splitter()
        : one_d_split_t {}
        , multi_d_split_t {}
    {
    }

    mixed_feature_splitter(one_d_algo_t one_d_algo, multi_d_algo_t multi_d_algo)
        : one_d_split_t { std::move(one_d_algo) }
        , multi_d_split_t { std::move(multi_d_algo) }
    {
    }

    template <mixed_feature_set_t mixede_feature_set_t, typename cost_fn_t>
    split_t split(
        const mixed_feature_set_t& feature_set,
        const labels& labels,
        cost_fn_t&& cost_fn) const
    {
        double best_single_feature_cost = std::numeric_limits<double>::max();
        auto single_result = split(
            best_single_feature_cost,
            feature_set.single_features(),
            labels,
            cost_fn);

        double best_multi_feature_cost = std::numeric_limits<double>::max();
        auto multi_result = split(
            best_multi_feature_cost,
            feature_set.multi_features(),
            labels,
            cost_fn);

        // FIXME - add some notion of complexity possibly when comparing
        return best_single_feature_cost > best_multi_feature_cost
            ? split_t{multi_result} : split_t{single_result};
    }
}; */

} // dtree
