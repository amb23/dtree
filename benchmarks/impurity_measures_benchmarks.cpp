
#include <benchmark/benchmark.h>

#include "dtree/impurity_measures.h"

void BM_gini_index(benchmark::State& state)
{
    using namespace dtree;

    std::vector<std::size_t> counts { 15'444, 32'123 };

    for (auto _ : state) {
        auto out = gini_index(counts);
        benchmark::DoNotOptimize(out);
    }
}

BENCHMARK(BM_gini_index);

BENCHMARK_MAIN();
