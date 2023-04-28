
#include <random>

#include "dtree/algos/roh_split.h"

namespace dtree::algos {

constexpr double epsilon = 1e-8;

std::vector<double> roh_split::generate_random_normal(std::size_t n_features) const
{
    std::vector<double> normal(n_features, 0.0);

    // FIXME - pass a seed to the constructor
    std::random_device rd {};
    std::mt19937 gen { rd() };

    std::normal_distribution<double> dist;

    bool all_zero = true;
    while (all_zero) {
        all_zero = true;
        for (std::size_t i = 0; i < n_features; ++i) {
            normal[i] = dist(gen);
            all_zero |= std::abs(normal[i]) < epsilon;
        }
    }

    return normal;
}

} // dtree::algos
