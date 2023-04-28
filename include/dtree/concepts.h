#pragma once

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
    t.size();

    t.begin();
    t.end();

    {
        t[std::declval<std::size_t>()]
        } -> std::convertible_to<double>;
};

template <typename T>
concept feature_set_c = requires(T t)
{
    requires feature_c<decltype(t[std::declval<feature_id>()])>;
};

/*
template <typename T>
concept FeatureSet = requires(T fs)
{

};*/

// Training concepts

} // dtree
