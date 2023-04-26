
#include <benchmark/benchmark.h>

#include "dtree/impurityMeasures.h"


void BM_GiniIndex(benchmark::State& state)
{
    using namespace dtree;

    std::vector<std::size_t> counts{15'444, 32'123};

    for (auto _ : state)
    {
        auto out = GiniIndex(counts);
        benchmark::DoNotOptimize(out);
    }
}

BENCHMARK(BM_GiniIndex);

BENCHMARK_MAIN();
