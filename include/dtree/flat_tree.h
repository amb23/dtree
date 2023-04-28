#pragma once

#include <variant>

#include "dtree/labels.h"
#include "dtree/utility.h"

namespace dtree {

class leaf {
public:
    leaf(label_distribution label_distribution)
        : m_label_distribution { std::move(label_distribution) }
    {
    }

    const label_distribution& get_distribution() const
    {
        return m_label_distribution;
    }

private:
    label_distribution m_label_distribution;
};

/// flat_tree
///
/// The simplest type of tree we have. It is potentially sparse if the tree
/// completes at other levels early.
/// For a tree of depth $n$ we could have at most $2^(n+1) - 1$ and this
/// model stores these nodes as a contiguous block. This does make searching
/// through the tree quite simple as we know where the next nodes are -
/// specifically if a node is at location $i$ then its children are at
/// $i << 1 + {1, 2}$. This means the class is essentially a set of functions
/// on the underlying container.
template <typename split_t>
class flat_tree {
    using node_t = std::variant<split_t, leaf>;
    using container_t = std::vector<node_t>;

public:
    using value_type = container_t::value_type;
    using reference = container_t::reference;
    using iterator = container_t::iterator;
    using size_type = container_t::size_type;

    static std::size_t get_depth(std::size_t loc)
    {
        return significant_power(loc + 1u);
    }

    static std::size_t next(bool lower, std::size_t loc)
    {
        return (loc << 1) + (lower ? 1u : 2u);
    }

    explicit flat_tree(std::size_t max_depth)
        : m_container((1 << (max_depth + 1)) - 1)
    {
    }

    explicit flat_tree(container_t container)
        : m_container { std::move(container) }
    {
    }

    decltype(auto) operator[](size_type loc) { return m_container[loc]; }
    decltype(auto) operator[](size_type loc) const { return m_container[loc]; }

    const container_t& data() const { return m_container; }

private:
    container_t m_container;
};

} // namespace dtree
