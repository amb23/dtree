#pragma once

#include <ostream>

#include "dtree/flat_tree.h"
#include "dtree/splittings.h"

namespace dtree {

class TreePrinter {
public:
    template <typename split_t>
    static void Print(std::ostream& os, const flat_tree<split_t>& tree)
    {
        Print(os, tree, 0);
    }

private:
    template <typename... Ts>
    struct Overloaded : public Ts... {
        using Ts::operator()...;
    };

    template <typename tree_t, typename... Ts>
    static void Print(std::ostream& os, const tree_t& tree, std::size_t loc)
    {
        std::visit(
            Overloaded {
                [&os, loc](const leaf& t) { Print(os, t, loc); },
                [&os, &tree, loc](const auto& t) {
                    Print(os, t, tree_t::get_depth(loc));
                    Print(os, tree, tree_t::next(true, loc));
                    Print(os, tree, tree_t::next(false, loc));
                } },
            tree[loc]);
    }

    static void Print(std::ostream&, const one_dimensional_split&, std::size_t depth);
    static void Print(std::ostream&, const leaf&, std::size_t depth);
};

} // dtree
