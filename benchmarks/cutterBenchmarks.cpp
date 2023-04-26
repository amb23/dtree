
#include <random>
#include <vector>

#include <benchmark/benchmark.h>

#include "dtree/detail/cuttingAlgorithms.h"
#include "dtree/impurityMeasures.h"
#include "dtree/labels.h"


template <typename Cutter>
void BM_Cutter(benchmark::State& state)
{
    using namespace dtree;

    std::size_t NSamples = state.range(0);

    std::mt19937 gen{}; // gen{rd()}
    std::normal_distribution<double> fDist;
    std::uniform_int_distribution<std::size_t> lDist{0, 1};

    std::vector<double> feature;
    Labels labels;

    for (std::size_t i = 0; i < NSamples; ++i)
    {
        feature.push_back(fDist(gen));
        labels.push_back(lDist(gen));
    }


    Cutter cutter;

    for (auto _ : state)
    {
        auto out = cutter(feature, labels, GiniIndex);
    }
}


BENCHMARK(BM_Cutter<dtree::detail::OptimalCutter>)
    -> RangeMultiplier(10)
    -> Range(100, 10'000'000)
    ;


BENCHMARK(BM_Cutter<dtree::detail::RandomCutter>)
    -> RangeMultiplier(10)
    -> Range(100, 10'000'000)
    ;


BENCHMARK(BM_Cutter<dtree::detail::MedianCutter>)
    -> RangeMultiplier(10)
    -> Range(100, 10'000'000)
    ;


BENCHMARK(BM_Cutter<dtree::detail::AverageCutter>)
    -> RangeMultiplier(10)
    -> Range(100, 10'000'000)
    ;


BENCHMARK_MAIN();
