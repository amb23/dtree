#pragma once

#include <tuple>
#include <variant>

#include <boost/mp11.hpp>

#include "dtree/concepts.h"
#include "dtree/labels.h"
#include "dtree/splittings.h"

namespace dtree::algos {

template <typename... algo_ts> class mixed_algo {
public:
    // FIXME - if there s only one type then just use that type rather that a variant
    //          on that type
    using splitting_type
        = boost::mp11::mp_unique<splitting_variant<typename algo_ts::splitting_type...>>;

    template <typename... arg_ts>
    explicit mixed_algo(arg_ts&&... args)
        : m_algos { std::forward<arg_ts>(args)... }
    {
    }

    template <mixed_feature_c mixed_feature_t, cost_fn_c cost_fn_t>
    std::pair<double, splitting_type> operator()(const mixed_feature_t& mixed_feature,
        const labels& labels_, cost_fn_t&& cost_fn) const
    {
        return std::visit(
            [this, &labels_, &cost_fn]<typename feature_t>(const feature_t& feature) {
                static_assert(has_invocable<algo_ts...>::template value<feature_t,
                                  labels, cost_fn_t>(),
                    "At least one of the mixed algos must be able to split the feature");
                double current_best = std::numeric_limits<double>::max();
                splitting_type current_splitting;
                return get_best_splitting<0>(
                    current_best, current_splitting, feature, labels_, cost_fn);
            },
            mixed_feature);
    }

private:
    template <std::size_t index, typename feature_t, cost_fn_c cost_fn_t>
    std::pair<double, splitting_type> get_best_splitting(double current_best,
        const splitting_type& current_splitting, const feature_t& feature,
        const labels& labels_, cost_fn_t&& cost_fn) const
    {
        if constexpr (index < sizeof...(algo_ts)) {
            using algo_t = element_t<index, algo_ts...>;
            if constexpr (std::is_invocable_v<algo_t, feature_t, labels, cost_fn_t>) {
                const auto& algo = std::get<index>(m_algos);
                auto [cost, splitting] = algo(feature, labels_, cost_fn);
                return get_best_splitting<index + 1>(
                    cost < current_best ? cost : current_best,
                    cost < current_best ? splitting_type { splitting }
                                        : current_splitting,
                    feature, labels_, cost_fn);
            } else {
                return get_best_splitting<index + 1>(
                    current_best, current_splitting, feature, labels_, cost_fn);
            }
        } else {
            return { current_best, current_splitting };
        }
    }

    std::tuple<algo_ts...> m_algos;
};

} // dtree::algos
