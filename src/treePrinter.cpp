
#include <iomanip>

#include "dtree/treePrinter.h"

namespace dtree {

void TreePrinter::Print(
    std::ostream& os,
    const one_dimensional_split& split,
    std::size_t depth)
{
    os << std::string(2 * depth, ' ')
       << "SPLIT: "
       << split.feature_id_
       << " @ "
       << std::fixed
       << std::setprecision(5)
       << split.split
       << std::endl;
}

void TreePrinter::Print(
    std::ostream& os,
    const leaf& leaf,
    std::size_t depth)
{
    os << std::string(2 * depth, ' ')
       << "LEAF: ";
    const auto& dist = leaf.get_distribution();
    for (std::size_t i = 0; i < dist.size(); ++i) {
        os << '[' << i << "] " << std::fixed << std::setprecision(5) << dist[i] << "  ";
    }
    os << std::endl;
}

} // dtree
