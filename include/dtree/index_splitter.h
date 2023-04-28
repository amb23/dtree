#pragma once

#include <variant>

#include "dtree/splittings.h"

namespace dtree {

/// Specialize this class for other splitting methods
template <typename split_t>
class index_splitter {
};

template <>
class index_splitter<one_dimensional_split> {
public:
    template <typename feature_set_t>
    std::vector<std::size_t> get_splitting_index(
        bool lower_split,
        const feature_set_t& features,
        const one_dimensional_split& split) const
    {
        const auto& splitting_feature = features.find(split.feature_id_)->second;

        std::vector<std::size_t> split_index;
        split_index.reserve(features.size());

        for (std::size_t i = 0; i < splitting_feature.size(); ++i) {
            double x = splitting_feature[i];
            if (x <= split.split == lower_split)
                split_index.push_back(i);
        }

        return split_index;
    }
};

template <typename... Ts>
class index_splitter<std::variant<Ts...>> {
private:
    using split_t = std::variant<Ts...>;

public:
    template <typename feature_set_t>
    std::vector<std::size_t> get_splitting_index(
        bool lower_split,
        const feature_set_t& features,
        const split_t& split) const
    {
        return std::visit(
            [lower_split, &features](const auto& v) {
                index_splitter<decltype(v)> index_splitter_;
                return index_splitter_.get_splitting_index(lower_split, features, v);
            },
            split);
    }
};

} // dtree
