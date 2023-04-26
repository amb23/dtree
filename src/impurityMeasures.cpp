
#include <cmath>
#include <numeric>

#include "dtree/impurityMeasures.h"

namespace dtree {


double GiniIndex(const LabelCounts& labelCounts)
{
    double totalCount = std::accumulate(
        begin(labelCounts),
        end(labelCounts),
        0u
    );

    double negGini = std::accumulate(
        begin(labelCounts),
        end(labelCounts),
        0.0,
        [totalCount](double g, auto count)
        {
            double p = static_cast<double>(count) / totalCount;
            return g + p*p;
        }
    );

    return 1.0 - negGini;
}


double Entropy(const LabelCounts& labelCounts)
{
    double totalCount = std::accumulate(
        begin(labelCounts),
        end(labelCounts),
        0u
    );

    double entropy = std::accumulate(
        begin(labelCounts),
        end(labelCounts),
        0.0,
        [totalCount](double e, auto count)
        {
            double p = static_cast<double>(count) / totalCount;
            return p > 0 ? e - p * std::log2(p) : e;
        }
    );

    return entropy;
}


} // namespace dtree
