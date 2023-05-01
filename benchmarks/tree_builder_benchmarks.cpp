

#include <random>
#include <unordered_map>
#include <vector>

#include <benchmark/benchmark.h>

#include "dtree/algos/single_numeric.h"
#include "dtree/impurity_measures.h"
#include "dtree/labels.h"
#include "dtree/tree_builder.h"

void run_test(dtree::tree_builder_config config, benchmark::State& state)
{
    using namespace dtree;

    std::size_t n_features = state.range(0);
    std::size_t n_samples = state.range(1);

    std::size_t max_depth = 10;

    // std::random_device rd{};
    std::mt19937 gen {}; // gen{rd()}

    std::unordered_map<std::size_t, std::vector<double>> X;

    std::normal_distribution<double> f_dist;

    for (std::size_t feature_id = 0; feature_id < n_features; ++feature_id) {
        std::vector<double> feature;
        feature.reserve(n_samples);
        for (std::size_t i = 0; i < n_samples; ++i) {
            feature.push_back(f_dist(gen));
        }

        X.emplace(feature_id, std::move(feature));
    }

    labels y;

    std::uniform_int_distribution<std::size_t> l_dist { 0, 1 };

    for (std::size_t i = 0; i < n_samples; ++i) {
        y.push_back(l_dist(gen));
    }

    tree_builder builder { config, algos::optimal_split {}, gini_index };

    for (auto _ : state) {
        auto out = builder.build(X, y);
    }
}

void BM_build_tree_sync(benchmark::State& state)
{
    run_test(dtree::tree_builder_config { false, 0u, 10u, 10u, 0.95 }, state);
}

void BM_build_tree_async(benchmark::State& state)
{
    run_test(dtree::tree_builder_config { true, 0u, 10u, 10u, 0.95 }, state);
}

void BM_build_tree_bounded_async(benchmark::State& state)
{
    run_test(dtree::tree_builder_config { true, 20'000u, 10u, 10u, 0.95 }, state);
}

BENCHMARK(BM_build_tree_sync)
    ->Args({ 1, 1'000 })
    ->Args({ 1, 10'000 })
    ->Args({ 1, 100'000 })
    ->Args({ 1, 1'000'000 })
    ->Args({ 10, 1'000 })
    ->Args({ 10, 10'000 })
    ->Args({ 10, 100'000 })
    ->Args({ 10, 1'000'000 })
    ->Args({ 100, 1'000 })
    ->Args({ 100, 10'000 })
    ->Args({ 100, 100'000 })
    //->Args({100, 1'000'000})
    ;
/*
BENCHMARK(BM_build_tree_async)
    ->Args({ 1, 1'000 })
    ->Args({ 1, 10'000 })
    ->Args({ 1, 100'000 })
    ->Args({ 1, 1'000'000 })
    ->Args({ 10, 1'000 })
    ->Args({ 10, 10'000 })
    ->Args({ 10, 100'000 })
    ->Args({ 10, 1'000'000 })
    ->Args({ 100, 1'000 })
    ->Args({ 100, 10'000 })
    ->Args({ 100, 100'000 })
    //->Args({100, 1'000'000})
    ;

BENCHMARK(BM_build_tree_bounded_async)
    ->Args({ 1, 1'000 })
    ->Args({ 1, 10'000 })
    ->Args({ 1, 100'000 })
    ->Args({ 1, 1'000'000 })
    ->Args({ 10, 1'000 })
    ->Args({ 10, 10'000 })
    ->Args({ 10, 100'000 })
    ->Args({ 10, 1'000'000 })
    ->Args({ 100, 1'000 })
    ->Args({ 100, 10'000 })
    ->Args({ 100, 100'000 })
    //->Args({100, 1'000'000})
    ;*/

BENCHMARK_MAIN();
