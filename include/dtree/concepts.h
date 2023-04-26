#pragma once

#include <cstdint>

namespace dtree {

// Data concepts

using FeatureID = std::size_t;

template <typename T>
concept SamplePoint = requires(T t)
{
    { t[std::declval<std::size_t>()] } -> std::convertible_to<double>;
};


/*
template <typename T>
concept FeatureSet = requires(T fs)
{
    
};*/


// Training concepts

} // dtree
