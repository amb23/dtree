
#include <cmath>
#include <numeric>

#include "dtree/impurity_measures.h"

namespace dtree {

double gini_index(const label_counts& label_counts)
{
    double total_count = std::accumulate(
        begin(label_counts),
        end(label_counts),
        0u);

    double neg_gini = std::accumulate(
        begin(label_counts),
        end(label_counts),
        0.0,
        [total_count](double g, auto count) {
            double p = static_cast<double>(count) / total_count;
            return g + p * p;
        });

    return 1.0 - neg_gini;
}

double entropy(const label_counts& label_counts)
{
    double total_count = std::accumulate(
        begin(label_counts),
        end(label_counts),
        0u);

    double entropy = std::accumulate(
        begin(label_counts),
        end(label_counts),
        0.0,
        [total_count](double e, auto count) {
            double p = static_cast<double>(count) / total_count;
            return p > 0 ? e - p * std::log2(p) : e;
        });

    return entropy;
}

} // namespace dtree
