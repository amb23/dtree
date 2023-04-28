
#include <iomanip>

#include "dtree/treePrinter.h"

namespace dtree {

void TreePrinter::Print(
    std::ostream& os, const one_dimensional_split& split, std::size_t depth)
{
    os << std::string(2 * depth, ' ') << "SPLIT: " << split.feature_id_ << " @ "
       << std::fixed << std::setprecision(5) << split.split << std::endl;
}

void TreePrinter::Print(
    std::ostream& os, const multi_dimensional_split& split, std::size_t depth)
{
    os << std::string(2 * depth, ' ') << "SPLIT: " << split.feature_id_ << " @ ";
    for (std::size_t i = 0u; i < split.normal.size(); ++i) {
        os << std::fixed << std::setprecision(4) << split.normal[i] << " * x_" << i;
        os << (i == split.normal.size() - 1 ? " <= " : " + ");
    }
    os << split.split << std::endl;
}

void TreePrinter::Print(std::ostream& os, const leaf& leaf, std::size_t depth)
{
    os << std::string(2 * depth, ' ') << "LEAF: ";
    const auto& dist = leaf.distribution();
    for (std::size_t i = 0; i < dist.size(); ++i) {
        os << '[' << i << "] " << std::fixed << std::setprecision(5) << dist[i] << "  ";
    }
    os << std::endl;
}

} // dtree
