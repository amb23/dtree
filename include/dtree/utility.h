#pragma once

namespace dtree {

// TODO - make work for other compilers
inline unsigned int significant_power(unsigned int x)
{
    constexpr int max = sizeof(unsigned int) * 8 - 1;
    return max - __builtin_clz(x);
}

inline unsigned int significant_power(unsigned long x)
{
    constexpr int max = sizeof(unsigned long) * 8 - 1;
    return max - __builtin_clzl(x);
}

} // dtree
