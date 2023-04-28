
#include <random>
#include <vector>

#include <benchmark/benchmark.h>

#include "dtree/algos/median_split.h"
#include "dtree/algos/optimal_split.h"
#include "dtree/impurity_measures.h"
#include "dtree/labels.h"

template <typename splitter_t> void BM_splitter(benchmark::State& state)
{
    using namespace dtree;

    std::size_t n_samples = state.range(0);

    std::mt19937 gen {};
    std::normal_distribution<double> f_dist;
    std::uniform_int_distribution<std::size_t> l_dist { 0, 1 };

    std::vector<double> feature;
    labels labels;

    for (std::size_t i = 0; i < n_samples; ++i) {
        feature.push_back(f_dist(gen));
        labels.push_back(l_dist(gen));
    }

    splitter_t splitter;

    for (auto _ : state) {
        auto out = splitter(feature, labels, gini_index);
    }
}

BENCHMARK(BM_splitter<dtree::algos::optimal_split>)
    ->RangeMultiplier(10)
    ->Range(100, 10'000'000);

BENCHMARK(BM_splitter<dtree::algos::median_split>)
    ->RangeMultiplier(10)
    ->Range(100, 10'000'000);

BENCHMARK_MAIN();
