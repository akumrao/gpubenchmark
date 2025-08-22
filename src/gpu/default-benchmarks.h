/*
 * List of test cases
 */


#ifndef GPULOAD_DEFAULT_BENCHMARKS_H_
#define GPULOAD_DEFAULT_BENCHMARKS_H_

#include <string>
#include <vector>
//#include <scene.h>
#include <util.h>

struct Config
{
    /* duration of interval in millsecond */
    int interval{200};
    /* 30 seconds duration of test */
    int duration{180};
    std::string temp{"/sys/class/thermal/thermal_zone22/temp"};

    uint64_t starttime;
    uint64_t curenttime{0};

    int GPUVendor{-1};
};



#include "base/logger.h"
using namespace base;


class DefaultBenchmarks
{
public:
    static const std::vector<std::string>& get( std::string &path, Config &config)
    {
        static std::vector<std::string> default_benchmarks;

        if (default_benchmarks.empty())
            populate(default_benchmarks, path, config);

        return default_benchmarks;
    }

private:

    static void pushback(std::vector<std::string>& benchmarks, json & m , std::string value)
    {
        benchmarks.push_back(value);
        m["gpu"].push_back(value) ;

    }

    static void saveback(std::vector<std::string>& benchmarks, json & m , std::string value)
    {
        m["gpu"].push_back(value) ;

    }

    static void populate(std::vector<std::string>& benchmarks, std::string &path, Config &config)
    {
        config.starttime = Util::get_timestamp_us();

        base::cnfg::Configuration conf;
        path +=  "config.json";
        conf.load(path);
        if( conf.root.is_null() )
       {

            conf.root["selected"] = "stress";
            conf.root["duration"] = 180; //second
            conf.root["interval"] = 200; // millisecond
            conf.root["temptNode"] = "/sys/class/thermal/thermal_zone22/temp";

            {
                json jtiles;
                jtiles["gpu"] = json::array();

                pushback(benchmarks, jtiles, "jellyfish:duration=5:validate=true");
                pushback(benchmarks, jtiles, "ideas:duration=5:validate=true");
                pushback(benchmarks, jtiles, "shading:duration=5:validate=true");
                pushback(benchmarks, jtiles ,"bump:duration=5:validate=true");
                pushback(benchmarks, jtiles, "shadow:duration=5:validate=true");
                pushback(benchmarks, jtiles,"build:duration=5:validate=true");
                pushback(benchmarks, jtiles,"terrain:duration=5:validate=true");
                pushback(benchmarks, jtiles,"conditionals:duration=5:validate=true");

                pushback(benchmarks, jtiles, "buffer:duration=20:validate=true");

                conf.root["test"]["tiles"] = jtiles;
            }


            {

                json throttle;
                throttle["throttle"] = json::array();

                throttle["gpu"] = json::array();
                
                saveback(benchmarks, throttle, "refract:duration=20:show-stats=false");
                saveback(benchmarks, throttle, "jellyfish:duration=20:show-stats=false");
                saveback(benchmarks, throttle, "build:duration=20:show-stats=false");
                saveback(benchmarks, throttle, "buffer:duration=20:show-stats=false");
                saveback(benchmarks, throttle, "refract:duration=20:show-stats=false");
                saveback(benchmarks, throttle, "jellyfish:duration=20:show-stats=false");
                saveback(benchmarks, throttle, "build:duration=20:show-stats=false");
                saveback(benchmarks, throttle, "buffer:duration=20:show-stats=false");

                conf.root["test"]["throttle"] = throttle;
            }


            {

                json cpu;
                cpu["cpu"] = json::array();

                saveback(benchmarks, cpu, "build:duration=20:show-stats=false");
                saveback(benchmarks, cpu, "refract:duration=20:show-stats=false");
                saveback(benchmarks, cpu, "jellyfish:duration=20:show-stats=false");
                saveback(benchmarks, cpu, "refract:duration=20:show-stats=false:show-power=false");
                saveback(benchmarks, cpu, "jellyfish:duration=20:show-stats=false:show-power=false");

                conf.root["test"]["cpu"] = cpu;
            }



            {
                json jstress;
                jstress["gpu"] = json::array();

                saveback(benchmarks, jstress, "build:duration=20:show-stats=false");
                saveback(benchmarks, jstress, "refract:duration=20:show-stats=false");
                saveback(benchmarks, jstress, "jellyfish:duration=20:show-stats=false");
                saveback(benchmarks, jstress, "refract:duration=20:show-stats=false:show-power=false");
                saveback(benchmarks, jstress, "jellyfish:duration=20:show-stats=false:show-power=false");

                conf.root["test"]["stress"] = jstress;
            }
            {

                json shader;
                shader["gpu"] = json::array();
                saveback(benchmarks, shader, "shading:duration=20:show-stats=false");
                conf.root["test"]["shader"] = shader;

            }

            {
                json pow;
                pow["gpu"] = json::array();
                saveback(benchmarks, pow, "terrain:duration=30");
                conf.root["test"]["pow"] = pow;
            }


            conf.save();


        }
        else
        {

            Level level = Level::Trace;
            Logger::instance().get("gpu_kpi")->setLevel(level);



            if( conf.root.find("duration")  != conf.root.end() )
                config.duration = conf.root["duration"].get<int>();

            if( conf.root.find("interval")  != conf.root.end() )
                config.interval = conf.root["interval"].get<int>();

            if( conf.root.find("temptNode")  != conf.root.end() )
               config.temp = conf.root["temptNode"].get<std::string>();;

            //SInfo << "conf " << conf.root.dump() << " path" << path;
            json::const_iterator jit;
            if( conf.root.find("selected")  != conf.root.end() ) {
                std::string sel = conf.root["selected"];

                for (jit = conf.root["test"][sel]["gpu"].begin();
                     jit != conf.root["test"][sel]["gpu"].end(); jit++) {
                    std::string str = jit->get<std::string>();
                    benchmarks.push_back(str);
                }
            }
        }

    }
};
//struct DefaultBenchmarks::Configuration DefaultBenchmarks::configuration;
#endif



/*
 Button buttonSetPortrait = (Button)findViewById(R.id.setPortrait);
Button buttonSetLandscape = (Button)findViewById(R.id.setLandscape);

buttonSetPortrait.setOnClickListener(new Button.OnClickListener(){

   @Override
   public void onClick(View arg0) {
        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);
   }

});

buttonSetLandscape.setOnClickListener(new Button.OnClickListener(){

   @Override
   public void onClick(View arg0) {
        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
   }

});
 *
 */
