
#include <random>

#include "dtree/algos/multi_numeric.h"

namespace dtree::algos {

constexpr double epsilon = 1e-8;

std::vector<double> generate_random_normal(std::size_t n_features, unsigned int seed)
{
    std::vector<double> normal(n_features, 0.0);

    std::mt19937 gen { seed == 0 ? std::random_device {}() : seed };

    std::normal_distribution<double> dist;

    bool all_zero = true;
    while (all_zero) {
        all_zero = true;
        for (std::size_t i = 0; i < n_features; ++i) {
            normal[i] = dist(gen);
            all_zero &= std::abs(normal[i]) < epsilon;
        }
    }

    return normal;
}

} // dtree::algos
