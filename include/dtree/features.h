#pragma once

#include <iterator>

#include "dtree/types.h"

namespace dtree {

template <typename base_iterator_t, typename container_t> class sub_range_iterator {
    using base_value_t = decltype(*std::declval<base_iterator_t>());

public:
    using difference_type = base_iterator_t::difference_type;
    using value_type = container_t::value_type;
    using reference
        = decltype(std::declval<container_t>()[std::declval<base_value_t>()]);
    using iterator_category = base_iterator_t::iterator_category;

    sub_range_iterator(base_iterator_t base, container_t& container)
        : m_base { base }
        , m_container { container }
    {
    }

    sub_range_iterator(const sub_range_iterator& rhs)
        : m_base { rhs.m_base }
        , m_container { rhs.m_container }
    {
    }

    bool operator==(const sub_range_iterator& rhs) const { return m_base == rhs.m_base; }

    sub_range_iterator& operator++()
    {
        ++m_base;
        return *this;
    }

    sub_range_iterator operator++(int)
    {
        sub_range_iterator out = *this;
        ++(*this);
        return out;
    }

    reference operator*() const { return m_container[*m_base]; }

    friend difference_type operator-(
        const sub_range_iterator& lhs, const sub_range_iterator& rhs)
    {
        return lhs.m_base - rhs.m_base;
    }

private:
    base_iterator_t m_base;
    container_t& m_container;
};

template <typename indexes_t, typename container_t> class sub_range_view {
    using index_iterator_t = indexes_t::const_iterator;

public:
    using iterator = sub_range_iterator<index_iterator_t, container_t>;
    using const_iterator = sub_range_iterator<index_iterator_t, const container_t>;
    using size_type = indexes_t::size_type;
    using value_type = container_t::value_type;
    using reference = container_t::reference;

    sub_range_view(const indexes_t& indexes, container_t& container)
        : m_indexes { indexes }
        , m_container { container }
    {
    }

    auto begin() { return iterator { m_indexes.begin(), m_container }; }
    auto end() { return iterator { m_indexes.end(), m_container }; }

    auto begin() const { return const_iterator { m_indexes.begin(), m_container }; }
    auto end() const { return const_iterator { m_indexes.end(), m_container }; }

    reference operator[](size_type loc)
    {
        using namespace std;
        auto it = m_indexes.begin();
        advance(it, loc);
        return m_container[*it];
    }

private:
    const indexes_t& m_indexes;
    container_t& m_container;
};

template <typename feature_set_t> class training_sample_point {
public:
    using sample_id_t = feature_set_t::sample_id_t;

    training_sample_point(sample_id_t sample_id, const feature_set_t& feature_set)
        : m_sample_id { sample_id }
        , m_feature_set { feature_set }
    {
    }

    double operator[](feature_id i) const { return m_feature_set[i][m_sample_id]; }

private:
    sample_id_t m_sample_id;
    const feature_set_t& m_feature_set;
};

} // dtree
