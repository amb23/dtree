#pragma once

#include <ostream>

#include <boost/serialization/split_free.hpp>
#include <boost/serialization/variant.hpp>
#include <boost/serialization/vector.hpp>

#include "dtree/flat_tree.h"
#include "dtree/splittings.h"
#include "dtree/types.h"

namespace boost::serialization {

template <typename archive_t, typename... Ts>
void save(archive_t& archive, const std::variant<Ts...>& v, unsigned int)
{
    using index_type = std::decay_t<decltype(v.index())>;
    index_type index = v.index();
    archive& make_nvp("index", index);
    std::visit([&archive](auto&& data) { archive& BOOST_SERIALIZATION_NVP(data); }, v);
}

struct variant_loader {
    template <std::size_t current_index, typename archive_t, typename... Ts>
    static void load(std::size_t index, archive_t& archive, std::variant<Ts...>& out)
    {
        if constexpr (current_index < sizeof...(Ts)) {
            if (index == current_index) {
                using data_t = dtree::element_t<current_index, Ts...>;
                data_t data;
                archive >> BOOST_SERIALIZATION_NVP(data);
                out = data;
            } else {
                variant_loader::load<current_index + 1>(index, archive, out);
            }
        } else {
            std::stringstream msg;
            // TODO - add type info to error msg
            msg << "Couldn't load variant with the given index: " << index;

            throw std::runtime_error { msg.str() };
        }
    }
};

template <typename archive_t, typename... Ts>
void load(archive_t& archive, std::variant<Ts...>& out, unsigned int)
{
    using index_type = std::decay_t<decltype(out.index())>;
    index_type index;
    archive >> make_nvp("index", index);

    variant_loader::load<0>(index, archive, out);
}

template <typename archive_t, typename... Ts>
void serialize(archive_t& archive, std::variant<Ts...>& out, unsigned int version)
{
    split_free(archive, out, version);
}

template <typename archive_t>
void serialize(
    archive_t& archive, dtree::single_numeric_splitting& splitting, unsigned int)
{
    archive& make_nvp("split", splitting.split);
}

template <typename archive_t>
void serialize(
    archive_t& archive, dtree::multi_numeric_splitting& splitting, unsigned int)
{
    archive& make_nvp("normal", splitting.normal);
    archive& make_nvp("split", splitting.split);
}

template <typename archive_t>
void serialize(
    archive_t& archive, dtree::has_substring_splitting& splitting, unsigned int)
{
    archive& make_nvp("substring", splitting.substring);
}

template <typename archive_t>
void serialize(
    archive_t& archive, dtree::string_length_splitting& splitting, unsigned int)
{
    archive& make_nvp("length", splitting.length);
}

template <typename archive_t, typename... splitting_ts>
void serialize(
    archive_t& archive, dtree::splitting_variant<splitting_ts...>& v, unsigned int)
{
    archive& make_nvp("splittings", v.splittings);
}

template <typename archive_t>
void save(archive_t& archive, const dtree::leaf& leaf, unsigned int)
{
    archive& make_nvp("distribution", leaf.distribution());
}

template <typename archive_t>
void load(archive_t& archive, dtree::leaf& leaf, unsigned int)
{
    dtree::label_distribution distribution;
    archive >> make_nvp("distribution", distribution);
    leaf = dtree::leaf { std::move(distribution) };
}

template <typename archive_t>
void serialize(archive_t& archive, dtree::leaf& leaf, unsigned int version)
{
    split_free(archive, leaf, version);
}

template <typename archive_t, typename splitting_t>
void serialize(archive_t& archive, dtree::node<splitting_t>& node, unsigned int)
{
    archive& make_nvp("feature_id", node.feature_id_);
    archive& make_nvp("splitting", node.splitting);
}

template <typename archive_t, typename split_t>
void save(archive_t& archive, const dtree::flat_tree<split_t>& tree, unsigned int)
{
    archive& make_nvp("data", tree.data());
}

template <typename archive_t, typename split_t>
void load(archive_t& archive, dtree::flat_tree<split_t>& tree, unsigned int)
{
    using container_t = dtree::flat_tree<split_t>::container_t;
    container_t container;
    archive >> make_nvp("data", container);
    tree = dtree::flat_tree<split_t> { std::move(container) };
}

template <typename archive_t, typename split_t>
void serialize(archive_t& archive, dtree::flat_tree<split_t>& tree, unsigned int version)
{
    split_free(archive, tree, version);
}

} // boost::serialization
