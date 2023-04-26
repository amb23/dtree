

#include <random>
#include <unordered_map>
#include <vector>

#include <benchmark/benchmark.h>

#include "dtree/cutters.h"
#include "dtree/impurityMeasures.h"
#include "dtree/labels.h"
#include "dtree/treeBuilder.h"

void RunTest(dtree::TreeBuilderConfig config, benchmark::State& state)
{
    using namespace dtree;

    std::size_t NFeatures = state.range(0);
    std::size_t NSamples = state.range(1);

    std::size_t MaxDepth = 10;

    // std::random_device rd{};
    std::mt19937 gen{}; // gen{rd()}

    std::unordered_map<std::size_t, std::vector<double>> features;

    std::normal_distribution<double> fDist;

    for (std::size_t featureID = 0; featureID < NFeatures; ++featureID)
    {
        std::vector<double> feature;
        feature.reserve(NSamples);
        for (std::size_t i = 0; i < NSamples; ++i)
        {
            feature.push_back(fDist(gen));
        }

        features.emplace(featureID, std::move(feature));
    }

    Labels labels;

    std::uniform_int_distribution<std::size_t> lDist{0, 1};

    for (std::size_t i = 0; i < NSamples; ++i)
    {
        labels.push_back(lDist(gen));
    }

    TreeBuilder builder{config, OptimalCut{}, GiniIndex};

    for (auto _ : state)
    {
        auto out = builder.Build(features, labels, StoppingCondition{MaxDepth, 10u, 0.95});
    }

}


void BM_BuildTreeSync(benchmark::State& state)
{
    RunTest(dtree::TreeBuilderConfig{false, 0u}, state);
}

void BM_BuildTreeAsync(benchmark::State& state)
{
    RunTest(dtree::TreeBuilderConfig{true, 0u}, state);
}

void BM_BuildTreeBoundedAsync(benchmark::State& state)
{
    RunTest(dtree::TreeBuilderConfig{true, 20'000u}, state);
}

BENCHMARK(BM_BuildTreeSync)
    ->Args({1, 1'000})
    ->Args({1, 10'000})
    ->Args({1, 100'000})
    ->Args({1, 1'000'000})
    ->Args({10, 1'000})
    ->Args({10, 10'000})
    ->Args({10, 100'000})
    ->Args({10, 1'000'000})
    ->Args({100, 1'000})
    ->Args({100, 10'000})
    ->Args({100, 100'000})
    //->Args({100, 1'000'000})
;

BENCHMARK(BM_BuildTreeAsync)
    ->Args({1, 1'000})
    ->Args({1, 10'000})
    ->Args({1, 100'000})
    ->Args({1, 1'000'000})
    ->Args({10, 1'000})
    ->Args({10, 10'000})
    ->Args({10, 100'000})
    ->Args({10, 1'000'000})
    ->Args({100, 1'000})
    ->Args({100, 10'000})
    ->Args({100, 100'000})
    //->Args({100, 1'000'000})
;

BENCHMARK(BM_BuildTreeBoundedAsync)
    ->Args({1, 1'000})
    ->Args({1, 10'000})
    ->Args({1, 100'000})
    ->Args({1, 1'000'000})
    ->Args({10, 1'000})
    ->Args({10, 10'000})
    ->Args({10, 100'000})
    ->Args({10, 1'000'000})
    ->Args({100, 1'000})
    ->Args({100, 10'000})
    ->Args({100, 100'000})
    //->Args({100, 1'000'000})
;

BENCHMARK_MAIN();
