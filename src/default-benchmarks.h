
#ifndef GPULOAD_DEFAULT_BENCHMARKS_H_
#define GPULOAD_DEFAULT_BENCHMARKS_H_

#include <string>
#include <vector>
#include <scene.h>
#include <util.h>

struct Config
{
    /* duration of interval in millsecond */
    int interval{200};
    /* 30 seconds duration of test */
    int duration{30};
    std::string temp{"/sys/class/thermal/thermal_zone10/temp"};

    uint64_t starttime;
    uint64_t curenttime{0};
};


class DefaultBenchmarks
{
public:
    static const std::vector<std::string>& get(Config &config)
    {
        static std::vector<std::string> default_benchmarks;

        if (default_benchmarks.empty())
            populate(default_benchmarks, config);

        return default_benchmarks;
    }

private:
    static void populate(std::vector<std::string>& benchmarks, Config &config)
    {
        config.starttime = Util::get_timestamp_us();

        benchmarks.push_back("build:use-vbo=false");
        benchmarks.push_back("build:use-vbo=true");
        benchmarks.push_back("texture:texture-filter=nearest");
        benchmarks.push_back("texture:texture-filter=linear");
        benchmarks.push_back("texture:texture-filter=mipmap");
        benchmarks.push_back("shading:shading=gouraud");
        benchmarks.push_back("shading:shading=blinn-phong-inf");
        benchmarks.push_back("shading:shading=phong");
        benchmarks.push_back("shading:shading=cel");
        benchmarks.push_back("bump:bump-render=high-poly");
        benchmarks.push_back("bump:bump-render=normals");
        benchmarks.push_back("bump:bump-render=height");
        benchmarks.push_back("effect2d:kernel=0,1,0;1,-4,1;0,1,0;");
        benchmarks.push_back("effect2d:kernel=1,1,1,1,1;1,1,1,1,1;1,1,1,1,1;");
        benchmarks.push_back("pulsar:quads=5:texture=false:light=false");
        benchmarks.push_back("desktop:windows=4:effect=blur:blur-radius=5:passes=1:separable=true");
        benchmarks.push_back("desktop:windows=4:effect=shadow");
        benchmarks.push_back("buffer:update-fraction=0.5:update-dispersion=0.9:columns=200:update-method=map:interleave=false");
        benchmarks.push_back("buffer:update-fraction=0.5:update-dispersion=0.9:columns=200:update-method=subdata:interleave=false");
        benchmarks.push_back("buffer:update-fraction=0.5:update-dispersion=0.9:columns=200:update-method=map:interleave=true");
        benchmarks.push_back("ideas:speed=duration");
        benchmarks.push_back("jellyfish");
        benchmarks.push_back("terrain");
        benchmarks.push_back("shadow");
        benchmarks.push_back("refract");
        benchmarks.push_back("conditionals:vertex-steps=0:fragment-steps=0");
        benchmarks.push_back("conditionals:vertex-steps=0:fragment-steps=5");
        benchmarks.push_back("conditionals:vertex-steps=5:fragment-steps=0");
        benchmarks.push_back("function:fragment-steps=5:fragment-complexity=low");
        benchmarks.push_back("function:fragment-steps=5:fragment-complexity=medium");
        benchmarks.push_back("loop:vertex-steps=5:fragment-steps=5:fragment-loop=false");
        benchmarks.push_back("loop:vertex-steps=5:fragment-steps=5:fragment-uniform=false");
        benchmarks.push_back("loop:vertex-steps=5:fragment-steps=5:fragment-uniform=true");
    }
};

#endif
