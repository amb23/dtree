#pragma once

#include <future>
#include <unordered_map>
#include <vector>

#include <spdlog/spdlog.h>

#include "dtree/features.h"
#include "dtree/flat_tree.h"
#include "dtree/index_splitter.h"
#include "dtree/labels.h"
#include "dtree/stop_condition.h"

namespace dtree {

struct tree_builder_config {
    bool async;
    std::size_t async_sample_min;
};

template <typename splitter_t, typename cost_fn_t>
class tree_builder {
public:
    using tree_t = flat_tree<typename splitter_t::split_t>;

    tree_builder(const tree_builder_config& config)
        : m_config { config }
        , m_splitter {}
        , m_cost_fn {}
    {
    }

    tree_builder(
        const tree_builder_config& config,
        splitter_t splitter,
        cost_fn_t cost_fn)
        : m_config { config }
        , m_splitter { std::move(splitter) }
        , m_cost_fn { std::move(cost_fn) }
    {
    }

    template <typename feature_set>
    tree_t build(
        const feature_set& features,
        const labels& labels_,
        const stop_condition& stop_condition) const
    {
        tree_t tree { stop_condition.get_max_depth() };
        build(tree, 0u, features, labels_, stop_condition);
        return tree;
    }

private:
    template <typename feature_set>
    void build(
        tree_t& tree,
        std::size_t loc,
        const feature_set& features,
        const labels& labels_,
        const stop_condition& stop_condition) const
    {
        if (auto reason = stop_condition.Check(tree_t::get_depth(loc), labels_)) {
            SPDLOG_DEBUG("Stopping building at depth {} as {}", depth, to_string(*reason));
            tree[loc] = leaf { labels_.calculate_distribution() };
        } else {
            auto split = m_splitter.split(features, labels_, m_cost_fn);
            SPDLOG_DEBUG(
                "Found optimal cut {} for feature {} with cost {}", cut, feature_id, cost);

            tree[loc] = split;

            build_sub_trees(
                tree,
                loc,
                features,
                labels_,
                stop_condition,
                split);
        }
    }

    template <typename feature_set, typename split_t>
    void build_sub_trees(
        tree_t& tree,
        std::size_t loc,
        const feature_set& features,
        const labels& labels_,
        const stop_condition& stop_condition,
        const split_t& split) const
    {
        if (m_config.async && labels_.size() > m_config.async_sample_min) {
            build_sub_trees_async(tree, loc, features, labels_, stop_condition, split);
        } else {
            build_sub_trees_sync(tree, loc, features, labels_, stop_condition, split);
        }
    }

    template <typename feature_set, typename split_t>
    void build_sub_trees_async(
        tree_t& tree,
        std::size_t loc,
        const feature_set& features,
        const labels& labels_,
        const stop_condition& stop_condition,
        const split_t& split) const
    {
        auto lower_future = std::async(
            [this, &tree, &features, &labels_, &stop_condition, split, loc]() {
                return build_sub_tree(tree, loc, true, features, labels_, stop_condition, split);
            });

        auto upper_future = std::async(
            [this, &tree, &features, &labels_, &stop_condition, split, loc]() {
                return build_sub_tree(tree, loc, false, features, labels_, stop_condition, split);
            });

        lower_future.get();
        upper_future.get();
    }

    template <typename feature_set, typename split_t>
    void build_sub_trees_sync(
        tree_t& tree,
        std::size_t loc,
        const feature_set& features,
        const labels& labels_,
        const stop_condition& stop_condition,
        const split_t& split) const
    {
        build_sub_tree(
            tree,
            loc,
            true,
            features,
            labels_,
            stop_condition,
            split);

        build_sub_tree(
            tree,
            loc,
            false,
            features,
            labels_,
            stop_condition,
            split);
    }

    template <typename feature_set, typename split_t>
    void build_sub_tree(
        tree_t& tree,
        std::size_t parent_loc,
        bool lower,
        const feature_set& features,
        const labels& labels_,
        const stop_condition& stop_condition,
        const split_t& split) const
    {
        index_splitter<split_t> index_splitter_;
        const auto& split_index = index_splitter_.get_splitting_index(lower, features, split);

        // We firstly need to take out the corresponding terms
        std::unordered_map<feature_id, std::vector<double>> split_features;
        for (const auto& [feature_id, feature] : features) {
            sub_range_view split_view { split_index, feature };
            std::vector<double> split_feature(begin(split_view), end(split_view));
            split_features.emplace(feature_id, move(split_feature));
        }

        sub_range_view split_view { split_index, labels_ };

        labels split_labels(split_view.begin(), split_view.end());

        std::size_t loc = tree_t::next(lower, parent_loc);

        build(tree, loc, split_features, split_labels, stop_condition);
    }

    tree_builder_config m_config;

    splitter_t m_splitter;

    cost_fn_t m_cost_fn;
};

} // dtree
