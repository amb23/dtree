#pragma once

#include <future>
#include <numeric>
#include <unordered_map>
#include <vector>

#include <spdlog/spdlog.h>

#include "dtree/concepts.h"
#include "dtree/flat_tree.h"
#include "dtree/labels.h"
#include "dtree/types.h"

namespace dtree {

struct tree_builder_config {
    bool async;
    std::size_t async_sample_min;
    /// stopping conditions
    std::size_t max_depth;
    std::size_t min_samples;
    double probability_limit;
};

template <typename algo_t, typename cost_fn_t> class tree_builder {
public:
    using node_type = node<typename algo_t::splitting_type>;
    using tree_t = flat_tree_t<algo_t>;

    tree_builder(const tree_builder_config& config)
        : m_config { config }
        , m_algo {}
        , m_cost_fn {}
    {
    }

    tree_builder(const tree_builder_config& config, algo_t algo, cost_fn_t cost_fn)
        : m_config { config }
        , m_algo { std::move(algo) }
        , m_cost_fn { std::move(cost_fn) }
    {
    }

    template <typename feature_set>
    tree_t build(const feature_set& features, const labels& labels_) const
    {
        tree_t tree { m_config.max_depth };
        build(tree, 0u, features, labels_);
        return tree;
    }

private:
    // TODO - document/code the return type
    // TODO - src?
    int should_stop(std::size_t current_depth, const labels& labels_) const
    {
        if (current_depth >= m_config.max_depth)
            return 101;
        if (labels_.size() <= m_config.min_samples)
            return 102;

        const auto& dist = labels_.calculate_distribution();
        auto max_p = std::max_element(begin(dist), end(dist));
        if (*max_p >= m_config.probability_limit)
            return 103;
        return 0;
    }

    template <typename feature_set>
    node_type find_split(const feature_set& features, const labels& labels_) const
    {
        double best_cost = std::numeric_limits<double>::max();
        typename algo_t::splitting_type best_split;
        feature_id best_feature_id = std::numeric_limits<feature_id>::max();

        for (const auto& [feature_id, feature] : features) {
            auto [cost, split] = m_algo(feature, labels_, m_cost_fn);
            SPDLOG_DEBUG(
                "Calculated split for feature {} with cost {}", feature_id, cost);
            if (cost < best_cost) {
                best_cost = cost;
                best_split = split;
                best_feature_id = feature_id;
            }
        }

        return { best_feature_id, best_split };
    }

    template <typename feature_set>
    std::vector<std::size_t> get_split_index(
        bool lower, const feature_set& features, const node_type& node) const
    {
        return do_index_split(lower, features.find(node.feature_id_)->second, node);
    }

    template <mixed_feature_set_c feature_set>
    std::vector<std::size_t> get_split_index(
        bool lower, const feature_set& features, const node_type& node) const
    {
        return std::visit(
            [this, &node, lower](
                const auto& feature) { return do_index_split(lower, feature, node); },
            features.find(node.feature_id_)->second);
    }

    template <typename feature_t>
    std::vector<std::size_t> do_index_split(
        bool lower, const feature_t& feature, const node_type& node) const
    {
        std::vector<std::size_t> split_index;
        split_index.reserve(feature.size());

        for (std::size_t i = 0; i < feature.size(); ++i) {
            const auto& x = feature[i];
            if (node.splitting(x) == lower)
                split_index.push_back(i);
        }

        return split_index;
    }

    template <typename feature_t>
    feature_t do_split_feature(
        const std::vector<std::size_t>& index, const feature_t& feature) const
    {
        feature_t split_feature;
        split_feature.reserve(index.size());
        for (std::size_t i : index)
            split_feature.push_back(feature[i]);
        return split_feature;
    }

    template <mixed_feature_c feature_t>
    feature_t do_split_feature(
        const std::vector<std::size_t>& index, const feature_t& feature) const
    {
        return std::visit(
            [this, &index](
                auto&& f) { return feature_t { do_split_feature(index, f) }; },
            feature);
    }

    template <typename feature_set>
    void build(tree_t& tree, std::size_t loc, const feature_set& features,
        const labels& labels_) const
    {
        if (auto reason = should_stop(tree_t::get_depth(loc), labels_)) {
            SPDLOG_DEBUG(
                "Stopping building at depth {} as {}", tree_t::get_depth(loc), reason);
            tree[loc] = leaf { labels_.calculate_distribution() };
        } else {
            auto split = find_split(features, labels_);
            SPDLOG_DEBUG("Best split found at depth {} on feature {}",
                tree_t::get_depth(loc), split.feature_id_);
            tree[loc] = split;

            build_sub_trees(tree, loc, features, labels_, split);
        }
    }

    template <typename feature_set>
    void build_sub_trees(tree_t& tree, std::size_t loc, const feature_set& features,
        const labels& labels_, const node_type& node) const
    {
        if (m_config.async && labels_.size() > m_config.async_sample_min) {
            build_sub_trees_async(tree, loc, features, labels_, node);
        } else {
            build_sub_trees_sync(tree, loc, features, labels_, node);
        }
    }

    template <typename feature_set>
    void build_sub_trees_async(tree_t& tree, std::size_t loc,
        const feature_set& features, const labels& labels_, const node_type& node) const
    {
        auto lower_future = std::async([this, &tree, &features, &labels_, node, loc]() {
            return build_sub_tree(tree, loc, true, features, labels_, node);
        });

        auto upper_future = std::async([this, &tree, &features, &labels_, node, loc]() {
            return build_sub_tree(tree, loc, false, features, labels_, node);
        });

        lower_future.get();
        upper_future.get();
    }

    template <typename feature_set>
    void build_sub_trees_sync(tree_t& tree, std::size_t loc, const feature_set& features,
        const labels& labels_, const node_type& node) const
    {
        build_sub_tree(tree, loc, true, features, labels_, node);

        build_sub_tree(tree, loc, false, features, labels_, node);
    }

    template <typename feature_set>
    void build_sub_tree(tree_t& tree, std::size_t parent_loc, bool lower,
        const feature_set& features, const labels& labels_, const node_type& node) const
    {
        auto split_index = get_split_index(lower, features, node);

        using feature_t = feature_set::mapped_type;
        std::unordered_map<feature_id, feature_t> split_features;
        for (const auto& [feature_id, feature] : features) {
            split_features.emplace(feature_id, do_split_feature(split_index, feature));
        }

        labels split_labels = do_split_feature(split_index, labels_);
        std::size_t loc = tree_t::next(lower, parent_loc);

        build(tree, loc, split_features, split_labels);
    }

    tree_builder_config m_config;

    algo_t m_algo;

    cost_fn_t m_cost_fn;
};

} // dtree
