#pragma once

#include <ostream>

#include "dtree/flat_tree.h"
#include "dtree/splittings.h"

namespace dtree {

template <typename T> class serializer {
public:
    static_assert(std::is_trivial_v<T>);

    void serialize(std::ostream& os, const T& t) const
    {
        os.write(reinterpret_cast<const char*>(&t), sizeof(T));
    }

    T deserialize(std::istream& is) const
    {
        T t;
        is.read(reinterpret_cast<char*>(&t), sizeof(T));
        return t;
    }
};

template <typename T> class serializer<std::vector<T>> {
public:
    void serialize(std::ostream& os, const std::vector<T>& data) const
    {
        using size_type = std::vector<T>::size_type;
        size_type size = data.size();
        os.write(reinterpret_cast<const char*>(&size), sizeof(size_type));
        for (const auto& elem : data) {
            m_elem_serializer.serialize(os, elem);
        }
    }

    std::vector<T> deserialize(std::istream& is) const
    {
        using size_type = std::vector<T>::size_type;
        size_type size;
        is.read(reinterpret_cast<char*>(&size), sizeof(size_type));

        std::vector<T> data;
        data.reserve(size);
        for (size_type i = 0; i < size; ++i) {
            data.push_back(m_elem_serializer.deserialize(is));
        }

        return data;
    }

private:
    serializer<T> m_elem_serializer;
};

template <typename... Ts> class serializer<std::variant<Ts...>> {
    using index_type = decltype(std::declval<std::variant<Ts...>>().index());
    static constexpr std::size_t n_indexes = sizeof...(Ts);

public:
    void serialize(std::ostream& os, const std::variant<Ts...>& data) const
    {
        index_type index = data.index();
        os.write(reinterpret_cast<const char*>(&index), sizeof(index));
        serialize<0>(os, index, data);
    }

    std::variant<Ts...> deserialize(std::istream& is) const
    {
        index_type index;
        is.read(reinterpret_cast<char*>(&index), sizeof(index_type));
        std::variant<Ts...> v;
        deserialize<0>(is, index, v);
        return v;
    }

private:
    template <index_type index_val>
    void serialize(
        std::ostream& os, index_type target, const std::variant<Ts...>& v) const
    {
        if constexpr (index_val < n_indexes) {
            if (index_val == target) {
                const auto& data = std::get<index_val>(v);
                serializer<std::decay_t<decltype(data)>> {}.serialize(os, data);
            } else {
                serialize<index_val + 1>(os, target, v);
            }
        }
    }

    template <index_type index_val>
    void deserialize(std::istream& is, index_type target, std::variant<Ts...>& v) const
    {
        if constexpr (index_val < n_indexes) {
            if (index_val == target) {
                using data_t = std::decay_t<decltype(std::get<index_val>(v))>;
                data_t data = serializer<data_t> {}.deserialize(is);
                v = data;
            } else {
                deserialize<index_val + 1>(is, target, v);
            }
        }
    }
};

template <> class serializer<leaf> {
public:
    void serialize(std::ostream& os, const leaf& leaf) const
    {
        m_impl.serialize(os, leaf.distribution());
    }

    leaf deserialize(std::istream& is) const { return leaf { m_impl.deserialize(is) }; }

private:
    using impl_t = std::decay_t<decltype(std::declval<leaf>().distribution())>;
    serializer<impl_t> m_impl;
};

template <> class serializer<multi_dimensional_split> {
    using type_1 = decltype(std::declval<multi_dimensional_split>().feature_id_);
    using type_2 = decltype(std::declval<multi_dimensional_split>().normal);
    using type_3 = decltype(std::declval<multi_dimensional_split>().split);

public:
    void serialize(std::ostream& os, const multi_dimensional_split& split) const
    {
        serializer<type_1> {}.serialize(os, split.feature_id_);
        serializer<type_2> {}.serialize(os, split.normal);
        serializer<type_3> {}.serialize(os, split.split);
    }

    multi_dimensional_split deserialize(std::istream& is) const
    {
        auto feature_id_ = serializer<type_1> {}.deserialize(is);
        auto normal = serializer<type_2> {}.deserialize(is);
        auto split = serializer<type_3> {}.deserialize(is);

        return { feature_id_, normal, split };
    }
};

template <typename split_t> class serializer<flat_tree<split_t>> {
public:
    void serialize(std::ostream& os, const flat_tree<split_t>& tree)
    {
        m_impl.serialize(os, tree.data());
    }

    flat_tree<split_t> deserialize(std::istream& is) const
    {
        return flat_tree<split_t> { m_impl.deserialize(is) };
    }

private:
    serializer<typename flat_tree<split_t>::container_t> m_impl;
};

template <typename T> void serialize(std::ostream& os, const T& t)
{
    serializer<T> {}.serialize(os, t);
}

template <typename T> T deserialize(std::istream& is)
{
    return serializer<T> {}.deserialize(is);
}

} // dtree
