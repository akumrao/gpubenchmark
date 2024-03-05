
#include "scene.h"
#include "log.h"
#include "shader-source.h"
#include "options.h"
#include "util.h"
#include <sstream>
#include <algorithm>

using std::stringstream;
using std::string;
using std::map;

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include "base/logger.h"
using namespace base;

void gpustats()
{
    static int nCount= 0;
    if(nCount == 180)
    {
        nCount = 1;
    }
    else if( nCount++ > 0 )
    {
        return;
    }

    {
        char txt[32] = {'\0'};
        int fd = open("/sys/devices/platform/1f000000.mali/cur_freq", O_RDONLY);
        if (fd > -1) {
            read(fd, txt, 32);
            STrace << "clock:" << txt;
            close(fd);

        }
    }

    {
        char txt[32] = {'\0'};
        int fd = open("/sys/devices/platform/1f000000.mali/utilization", O_RDONLY);
        if (fd > -1) {
            read(fd, txt, 32);
            STrace << "utilization:" << txt;
            close(fd);

        }
    }

    {
        char txt[32] = {'\0'};
        int fd = open("/sys/class/thermal/thermal_zone10/temp", O_RDONLY);
        if (fd > -1) {
            read(fd, txt, 32);
            STrace << "thermal_zone10:" << txt;
            close(fd);

        }
    }


}



void gpuinit()
{

    {
        char txt[512] = {'\0'};
        int fd = open("/sys/devices/platform/1f000000.mali/clock_info", O_RDONLY);
        if (fd > -1) {
            read(fd, txt, 511);
            STrace << "clock_info:" << txt;
            close(fd);

        }
    }

}

Scene::Option::Option(const std::string &nam, const std::string &val, const std::string &desc,
                      const std::string &values) :
name(nam), value(val), default_value(val), description(desc), set(false)
{
    Util::split(values, ',', acceptable_values, Util::SplitModeNormal);
}

Scene::Scene(Canvas &pCanvas, const string &name) :
    canvas_(pCanvas), name_(name),
    startTime_(0), lastUpdateTime_(0), currentFrame_(0),
    running_(0), duration_(0), nframes_(0)
{
    options_["duration"] = Scene::Option("duration", "10.0",
                                         "The duration of each benchmark in seconds");
    options_["nframes"] = Scene::Option("nframes", "",
                                         "The number of frames to render");
    options_["vertex-precision"] = Scene::Option("vertex-precision",
                                                 "default,default,default,default",
                                                 "The precision values for the vertex shader (\"int,float,sampler2d,samplercube\")");
    options_["fragment-precision"] = Scene::Option("fragment-precision",
                                                   "default,default,default,default",
                                                   "The precision values for the fragment shader (\"int,float,sampler2d,samplercube\")");
    /* FPS options */
    options_["show-fps"] = Scene::Option("show-fps", "false",
                                         "Show live FPS counter",
                                         "false,true");
    options_["fps-pos"] = Scene::Option("fps-pos", "-1.0,-1.0",
                                         "The position on screen where to show FPS");
    options_["fps-size"] = Scene::Option("fps-size", "0.03",
                                         "The width of each glyph for the FPS");
    /* Title options */
    options_["title"] = Scene::Option("title", "",
                                      "The scene title to show");
    options_["title-pos"] = Scene::Option("title-pos", "-0.7,-1.0",
                                      "The position on screen where to show the title");
    options_["title-size"] = Scene::Option("title-size", "0.03",
                                           "The width of each glyph in the title");
}

Scene::~Scene()
{
}

bool
Scene::supported(bool show_errors)
{
    static_cast<void>(show_errors);
    return true;
}

bool
Scene::load()
{
    return true;
}

void
Scene::unload()
{
}

bool
Scene::setup()
{
    duration_ = Util::fromString<double>(options_["duration"].value);
    nframes_ = Util::fromString<unsigned>(options_["nframes"].value);

    ShaderSource::default_precision(
            ShaderSource::Precision(options_["vertex-precision"].value),
            ShaderSource::ShaderTypeVertex
            );

    ShaderSource::default_precision(
            ShaderSource::Precision(options_["fragment-precision"].value),
            ShaderSource::ShaderTypeFragment
            );

    currentFrame_ = 0;
    running_ = false;
    startTime_ = Util::get_timestamp_us() / 1000000.0;
    lastUpdateTime_ = startTime_;

    return supported(true);
}

void
Scene::teardown()
{
}

void
Scene::update()
{
    double current_time = Util::get_timestamp_us() / 1000000.0;
    double elapsed_time = current_time - startTime_;

    currentFrame_++;

    lastUpdateTime_ = current_time;

    if (elapsed_time >= duration_)
        running_ = false;

    if (nframes_ > 0 && currentFrame_ >= nframes_)
        running_ = false;
}

void
Scene::draw()
{
}

string
Scene::info_string(const string &title)
{
    stringstream ss;

    ss << "[" << name_ << "] " << Scene::construct_title(title);

    return ss.str();
}

unsigned
Scene::average_fps()
{
    double elapsed_time = lastUpdateTime_ - startTime_;
    return currentFrame_ / elapsed_time;
}

bool
Scene::set_option(const string &opt, const string &val)
{
    map<string, Option>::iterator iter = options_.find(opt);

    if (iter == options_.end())
        return false;

    std::vector<std::string> &values(iter->second.acceptable_values);

    if (!values.empty() && 
        std::find(values.begin(), values.end(), val) == values.end())
    {
            return false;
    }

    iter->second.value = val;
    iter->second.set = true;

    return true;
}

void
Scene::reset_options()
{
    for (map<string, Option>::iterator iter = options_.begin();
         iter != options_.end();
         iter++)
    {
        Option &opt = iter->second;

        opt.value = opt.default_value;
        opt.set = false;
    }
}

bool
Scene::set_option_default(const string &opt, const string &val)
{
    map<string, Option>::iterator iter = options_.find(opt);

    if (iter == options_.end())
        return false;

    std::vector<std::string> &values(iter->second.acceptable_values);

    if (!values.empty() && 
        std::find(values.begin(), values.end(), val) == values.end())
    {
            return false;
    }

    iter->second.default_value = val;

    return true;
}


string
Scene::construct_title(const string &title)
{
    stringstream ss;

    if (title == "") {
        for (map<string, Option>::iterator iter = options_.begin();
             iter != options_.end();
             iter++)
        {
            if (Options::show_all_options || iter->second.set)
            {
                ss << iter->first << "=" << iter->second.value << ":";
            }
        }

        if (ss.str().empty())
            ss << "<default>:";
    }
    else
        ss << title;

    return ss.str();

}

bool
Scene::load_shaders_from_strings(Program &program,
                                 const std::string &vtx_shader,
                                 const std::string &frg_shader,
                                 const std::string &vtx_shader_filename,
                                 const std::string &frg_shader_filename)
{
    program.init();

    Log::debug("Loading vertex shader from file %s:\n%s",
               vtx_shader_filename.c_str(), vtx_shader.c_str());

    program.addShader(GL_VERTEX_SHADER, vtx_shader);
    if (!program.valid()) {
        Log::error("Failed to add vertex shader from file %s:\n  %s\n",
                   vtx_shader_filename.c_str(),
                   program.errorMessage().c_str());
        program.release();
        return false;
    }

    Log::debug("Loading fragment shader from file %s:\n%s",
               frg_shader_filename.c_str(), frg_shader.c_str());

    program.addShader(GL_FRAGMENT_SHADER, frg_shader);
    if (!program.valid()) {
        Log::error("Failed to add fragment shader from file %s:\n  %s\n",
                   frg_shader_filename.c_str(),
                   program.errorMessage().c_str());
        program.release();
        return false;
    }

    program.build();
    if (!program.ready()) {
        Log::error("Failed to link program created from files %s and %s:  %s\n",
                   vtx_shader_filename.c_str(),
                   frg_shader_filename.c_str(),
                   program.errorMessage().c_str());
        program.release();
        return false;
    }

    return true;
}

void Scene::statsInit()
{
    Log::info("statsInit");
    gpuinit();
}
void Scene::statsRun()
{
    gpustats();
}
void Scene::statsStop(){
    Log::info("statsStop");
    gpuinit();
}
