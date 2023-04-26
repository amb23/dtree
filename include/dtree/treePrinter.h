#pragma once

#include <ostream>

#include "dtree/tree.h"

namespace dtree {

class TreePrinter
{
public:

    static void Print(std::ostream&, const Node&, std::size_t depth = 0);

private:

    static void Print(std::ostream&, const Branch&, std::size_t depth);
    static void Print(std::ostream&, const Leaf&, std::size_t depth);
};




} // dtree
