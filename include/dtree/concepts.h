#pragma once

#include <ranges>

#include "dtree/types.h"

namespace dtree {

// Data concepts

template <typename T>
concept sample_point = requires(T t)
{
    {
        t[std::declval<feature_id>()]
        } -> std::convertible_to<double>;
};

template <typename T>
concept feature_c = requires(T t)
{
    requires std::ranges::sized_range<T>;

    {
        t[std::declval<std::size_t>()]
        } -> std::convertible_to<double>;
};

template <typename T>
concept feature_set_c = requires(T t)
{
    requires feature_c<decltype(t[std::declval<feature_id>()])>;
};

/// multi_feature_c
///
/// A multi_feature must return a range at each sample. A common example would
/// be the measurements of the location of an object in R^D, or some family of
/// common technical indicators e.g. moving averages.
template <typename T>
concept multi_feature_c = requires(T t)
{
    requires std::ranges::sized_range<T>;
    requires std::ranges::range<decltype(t[std::declval<std::size_t>()])>;
};

template <typename T>
concept multi_feature_set_c = requires(T t)
{
    requires multi_feature_c<decltype(t[std::declval<feature_id>()])>;
};

/// mixed_feature_set_c
///
/// A mixed feature set allows one to run both single and multi feature
/// analysis.
template <typename T>
concept mixed_feature_set_c = requires(T t)
{
    requires feature_set_c<decltype(t.single_features())>;
    requires multi_feature_set_c<decltype(t.multi_features())>;
};

} // dtree
