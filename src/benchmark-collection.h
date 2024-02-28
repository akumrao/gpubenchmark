
#ifndef GPULOAD_BENCHMARK_COLLECTION_H_
#define GPULOAD_BENCHMARK_COLLECTION_H_

#include <vector>
#include <string>
#include "benchmark.h"

class BenchmarkCollection
{
public:
    BenchmarkCollection() {}
    ~BenchmarkCollection();

    /*
     * Adds benchmarks to the collection.
     */
    void add(const std::vector<std::string> &benchmarks);

    /*
     * Populates the collection guided by the global options.
     */
    void populate_from_options();

    /*
     * Whether the benchmarks in this collection need decoration.
     */
    bool needs_decoration();

    const std::vector<Benchmark *>& benchmarks() { return benchmarks_; }

private:
    void add_benchmarks_from_files();
    bool benchmarks_contain_normal_scenes();

    std::vector<Benchmark *> benchmarks_;
};

#endif /* GPULOAD_BENCHMARK_COLLECTION_H_ */
