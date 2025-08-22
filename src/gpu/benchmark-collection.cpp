/*
 * Class for Benchmark Storage with Vector
 *
 */


#include <fstream>
#include "benchmark-collection.h"
#include "default-benchmarks.h"
#include "options.h"
#include "log.h"
#include "util.h"

BenchmarkCollection::~BenchmarkCollection()
{
    Util::dispose_pointer_vector(benchmarks_);
}

void
BenchmarkCollection::add(const std::vector<std::string> &benchmarks)
{
    for (std::vector<std::string>::const_iterator iter = benchmarks.begin();
         iter != benchmarks.end();
         iter++)
    {
        benchmarks_.push_back(new Benchmark(*iter));
    }
}

void
BenchmarkCollection::populate_from_options( std::string path)
{
    if (Options::annotate) {
        std::vector<std::string> annotate;
        annotate.push_back(":show-stats=true:title=#info#");
        add(annotate);
    }

    if (!Options::benchmarks.empty())
        add(Options::benchmarks);

    if (!Options::benchmark_files.empty())
        add_benchmarks_from_files();

    if (!benchmarks_contain_normal_scenes())
        add(DefaultBenchmarks::get(path, config));
}

bool
BenchmarkCollection::needs_decoration()
{
    for (std::vector<Benchmark *>::const_iterator bench_iter = benchmarks_.begin();
         bench_iter != benchmarks_.end();
         bench_iter++)
    {
        const Benchmark *bench = *bench_iter;
        if (bench->needs_decoration())
            return true;
    }

    return false;
}


void
BenchmarkCollection::add_benchmarks_from_files()
{
    for (std::vector<std::string>::const_iterator iter = Options::benchmark_files.begin();
         iter != Options::benchmark_files.end();
         iter++)
    {
        std::ifstream ifs(iter->c_str());

        if (!ifs.fail()) {
            std::string line;

            while (getline(ifs, line)) {
                if (!line.empty())
                    benchmarks_.push_back(new Benchmark(line));
            }
        }
        else {
            Log::error("Cannot open benchmark file %s\n",
                       iter->c_str());
        }

    }
}

bool
BenchmarkCollection::benchmarks_contain_normal_scenes()
{
    for (std::vector<Benchmark *>::const_iterator bench_iter = benchmarks_.begin();
         bench_iter != benchmarks_.end();
         bench_iter++)
    {
        const Benchmark *bench = *bench_iter;
        if (!bench->scene().name().empty())
            return true;
    }

    return false;
}

