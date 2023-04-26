
#include <iomanip>

#include "dtree/treePrinter.h"


namespace dtree {

void TreePrinter::Print(
    std::ostream& os,
    const Node& node,
    std::size_t depth)
{
    std::visit([&os, depth] (const auto& v) { Print(os, v, depth); }, node);
}

void TreePrinter::Print(
    std::ostream& os,
    const Branch& branch,
    std::size_t depth)
{
    os << std::string(2 * depth, ' ')
        << "BRANCH: "
        << "Cut="
        << std::fixed
        << std::setprecision(5)
        << branch.GetCut()
        << " @ "
        << branch.GetFeatureID()
        << std::endl;

    Print(os, branch.GetLower(), depth + 1);
    Print(os, branch.GetUpper(), depth + 1);
}

void TreePrinter::Print(
    std::ostream& os,
    const Leaf& leaf,
    std::size_t depth)
{
    os << std::string(2 * depth, ' ')
        << "LEAF: ";
    const auto& dist = leaf.GetDistribution();
    for (std::size_t i = 0; i < dist.size(); ++i)
    {
        os << '[' << i << "] " << std::fixed << std::setprecision(5) << dist[i] << "  ";
    }
    os << std::endl;
}



} // dtree
