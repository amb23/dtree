#pragma once

#include <ranges>
#include <tuple>
#include <variant>

#include "dtree/types.h"

namespace dtree {

////////////////////////////////////////////////////////////////////////////////////////
/// General helpers

template <std::size_t index, typename... Ts>
using element_t = std::tuple_element_t<index, std::tuple<Ts...>>;

template <typename container_t> struct value_type_detail {
    // for some reason ranges don't have a value type so we use this instead
    using type = std::decay_t<decltype(*begin(std::declval<container_t>()))>;
};

template <typename container_t> using value_type = value_type_detail<container_t>::type;

template <typename... invocable_ts> struct has_invocable {
    template <typename... arg_ts> static constexpr bool value()
    {
        return (std::is_invocable_v<invocable_ts, arg_ts...> || ...);
    }
};

////////////////////////////////////////////////////////////////////////////////////////
/// Data concepts

template <typename T>
concept single_numeric_sample_c = std::is_arithmetic_v<T>;

/// multi_numeric_sample_c
///
/// Represents a multi dimensional observation. A simple example would be the
/// measurements of the location of an object in R^D, or some family of common technical
/// indicators e.g. moving averages. There is an additional contract not in the concept
/// that each sample of a given feature has the same dimension.
template <typename T>
concept multi_numeric_sample_c = requires
{
    requires std::ranges::sized_range<T>;
    requires single_numeric_sample_c<typename T::value_type>;
};

template <typename T>
concept single_string_sample_c = std::convertible_to<T, std::string>;

/// *_feature_c
///
/// Basic feature types that hold training data of a single sample type
template <typename T>
concept single_numeric_feature_c
    = std::ranges::sized_range<T> && single_numeric_sample_c<value_type<T>>;

template <typename T>
concept multi_numeric_feature_c
    = std::ranges::sized_range<T> && multi_numeric_sample_c<value_type<T>>;

template <typename T>
concept string_feature_c
    = std::ranges::sized_range<T> && single_string_sample_c<value_type<T>>;

/// *_feature_set_c
///
/// The basic feature sets is a collection of features of the same type. All the feature
/// sets should be considered as a map { feature_id -> feature }. In general we don't
/// specialize anywhere on these types, just on the mixed_feature_set_c (see below)
// TODO - need to add a find method and other general map methods to the feature set
// concept
template <typename T>
concept single_numeric_feature_set_c = single_numeric_feature_c<
    typename T::mapped_type> && std::is_convertible_v<feature_id, typename T::key_type>;

template <typename T>
concept multi_numeric_feature_set_c = multi_numeric_feature_c<
    typename T::mapped_type> && std::is_convertible_v<feature_id, typename T::key_type>;

template <typename T>
concept string_feature_set_c
    = string_feature_c<typename T::mapped_type> && std::is_convertible_v<feature_id,
        typename T::key_type>;

/// mixed_feature_c
/// mixed_feature_set_c
///
/// A mixed feature set allows one to run analysis on data sets that have hetrogeneous
/// data types. As throughout the repo we use a variant to define the allowed feature
/// types. An example of a mixed feature would be:
///     variant<vector<double>, vector<array<double, 3>>, ...>
/// User defined types can be added to mixed feature. All fitting should be done using
/// the dtree::algos::mixed_algo or something of a similar ilk.
/// Note that the output of the fitting for a mixed feature set is slightly more complex
/// than when the features are all of the same type. One must ensure that the data in
/// testing/production has the same semantics as in training (although this should hold
/// by construction the type system here doesn't enforce it).
/// See also dtree::algos::mixed_algo, dtree::splitting_variant for more details on
/// these points.
template <typename T> struct is_mixed_feature {
    static constexpr bool value() { return false; }
};

template <typename... Ts> struct is_mixed_feature<std::variant<Ts...>> {
    static constexpr bool value() { return (std::ranges::sized_range<Ts> && ...); }
};

template <typename T>
concept mixed_feature_c = is_mixed_feature<T>::value();

template <typename T>
concept mixed_feature_set_c
    = mixed_feature_c<typename T::mapped_type> && std::convertible_to<feature_id,
        typename T::key_type>;

////////////////////////////////////////////////////////////////////////////////////////
/// Algo concepts

template <typename cost_fn_t>
concept cost_fn_c = std::is_invocable_r_v<double, cost_fn_t, const label_counts&>;

template <typename splitting_t, typename feature_t>
inline constexpr bool splits_v
    = std::is_invocable_r_v<bool, splitting_t, const feature_t&>;

} // dtree
