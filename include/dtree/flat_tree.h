#pragma once

#include <variant>

#include "dtree/labels.h"
#include "dtree/splittings.h"
#include "dtree/utility.h"

namespace dtree {

class leaf {
public:
    // TODO - remove this perhaps
    leaf()
        : m_label_distribution {}
    {
    }

    explicit leaf(label_distribution label_distribution)
        : m_label_distribution { std::move(label_distribution) }
    {
    }

    bool operator==(const leaf&) const = default;

    const label_distribution& distribution() const { return m_label_distribution; }

private:
    label_distribution m_label_distribution;
};

/// flat_tree
///
/// The simplest type of tree we have. It is quite wasteful if the tree completes at
/// depths significantly before the max depth. For a tree of depth $n$ we have at most
/// $2^(n+1) - 1$ nodes and this model stores them as a contiguous block. This does make
/// searching through the tree quite simple as we know where the next nodes are -
/// specifically if a node is at location $i$ then its children are at $i << 1 + {1|2}$.
template <typename split_t> class flat_tree {
    // TODO - do we want to make sure split_t = node<split_t>??
    //      - or do we wrap in a node here?
    using node_t = std::variant<split_t, leaf>;

public:
    using container_t = std::vector<node_t>;
    using value_type = container_t::value_type;
    using reference = container_t::reference;
    using iterator = container_t::iterator;
    using size_type = container_t::size_type;

    static std::size_t get_depth(std::size_t loc) { return significant_power(loc + 1u); }

    static std::size_t next(bool lower, std::size_t loc)
    {
        return (loc << 1) + (lower ? 1u : 2u);
    }

    flat_tree()
        : m_container {}
    {
    }

    explicit flat_tree(std::size_t max_depth)
        : m_container((1 << (max_depth + 1)) - 1)
    {
    }

    explicit flat_tree(container_t container)
        : m_container { std::move(container) }
    {
    }

    // TODO - this should not be defaulted as anything after a leaf is unreachable
    bool operator==(const flat_tree&) const = default;

    decltype(auto) operator[](size_type loc) { return m_container[loc]; }
    decltype(auto) operator[](size_type loc) const { return m_container[loc]; }

    const container_t& data() const { return m_container; }

    template <typename sample_t>
    const label_distribution& apply(const sample_t& sample) const
    {
        for (size_type loc = 0;;) {
            const auto& node = m_container[loc];
            if (std::holds_alternative<leaf>(node)) {
                return std::get<leaf>(node).distribution();
            }

            const auto& split = std::get<split_t>(node);
            loc = next(split.splitting(sample[split.feature_id_]), loc);
        }
    }

private:
    container_t m_container;
};

} // namespace dtree
