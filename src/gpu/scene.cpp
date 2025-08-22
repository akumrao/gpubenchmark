/*
 *  Base calls for all the the scenes, all the logic for tiles, wallpaper, golden frame buffer comparison are done here
 *
 */
#include "scene.h"
#include "log.h"
#include "shader-source.h"
#include "options.h"
#include "util.h"
#include <sstream>
#include <algorithm>
#include "base/time.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include "base/logger.h"
#include "json/json.h"
#include <regex>

using std::stringstream;
using std::string;
using std::map;


using namespace base;

std::vector<std::string> splitString(const std::string &str) // https://www.techiedelight.com/split-a-string-on-newlines-in-cpp/
{
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, '\n'))
    {
        tokens.push_back(token);
    }
    return tokens;
}

bool findBetween(  const std::string str, const std::string start , const std::string end , int &ret)
{
    std::string::size_type first = str.find_last_of(start);
    if(first  ==   std::string::npos  )
        return false;

    std::string::size_type last = str.find_last_of(end);

    if(last ==   std::string::npos  )
        return false;

    std::string strNew = str.substr (first, last-first);

    ret = std::stoi(strNew);
    return true;
}

bool readskipNthLine(const std::string filename, int N , int &ret)
{
    bool status = false;

    std::ifstream in(filename.c_str());

    if (!in.is_open()) {
        std::cerr << "Error opening the file!";
        return 0;
    }


    std::string s;
    //for performance
    //s.reserve(some_reasonable_max_line_length);

    //skip N lines
    for(int i = 0; i < N; ++i)
        std::getline(in, s);


    while (getline(in, s))
    {
        status = findBetween(s, "GPU Utilisation", "%" ,ret);
        if(status)
            break;
    }

    return status;
}


void Scene::gpustats(bool showFullStats, Config& config)
{
    unsigned fps =  average_fps();
    if(!showFullStats)
    {
        STrace <<   " fps " <<  average_fps() <<  " frametime " <<  (fps ?  1000.0 / fps:0);
        return;
    }


//    static int nCount= 0;
//    if(nCount == 90)
//    {
//        nCount = 1;
//    }
//    else if( nCount++ > 0 )
//    {
//        return;
//    }

    //json &gpuNode =    Scene::stats.root["gpu"];

//    struct timeval tp;
//    gettimeofday(&tp, NULL);
//
//    long int diffms =0 ;
//
//    if(!ms)
//    {
//        ms = tp.tv_sec * 1000 + tp.tv_usec / 1000; //get current timestamp in milliseconds
//        //gpuNode["ts"]= std::to_string(ms);
//    }
//    else
//    {
//        diffms = (tp.tv_sec * 1000 + tp.tv_usec / 1000) - ms;
//    }


    {
        char txt[32] = {'\0'};

        int fd;
        if(config.GPUVendor == 1)
        {
             fd = open("/sys/devices/platform/1f000000.mali/utilization", O_RDONLY);
               if (fd > -1) {
                read(fd, txt, 31);
                utilization = std::stoi(txt );

                close(fd);

//                json &utilNode =    Scene::stats.root["gpu"]["util"];
//
//                json obj = json::object();
//               // obj[std::to_string(diffms)] = ;
//                obj["ts"] = diffms;
//                obj["per"] = utilization;
//                utilNode.push_back(obj);
 	      }

        }
        else
        {
             readskipNthLine("/proc/pvr/status", 4, utilization);
        }
    }



    {
        char txt[32] = {'\0'};

        int fd;
        if(config.GPUVendor == 1) {
            fd = open("/sys/devices/platform/1f000000.mali/cur_freq", O_RDONLY);
        }
        else
            fd = open("/sys/devices/platform/34f00000.gpu0/devfreq/34f00000.gpu0/cur_freq", O_RDONLY);

        if (fd > -1) {
            read(fd, txt, 31);
            cur_freq = std::stoi(txt );
            cur_freq_per =  float(( cur_freq - min_freq) * 100)/float( max_freq -  min_freq );
            cur_freq = cur_freq/1000;
            close(fd);

//            json &clkNode =    Scene::stats.root["gpu"]["clk"];
//
//            json obj = json::object();
//            //obj[std::to_string(diffms)] = (int)cur_freq_per;
//            obj["ts"] = diffms;
//            obj["fr"] = (int)cur_freq_per;
//            clkNode.push_back(obj);

        }
    }


    {
        char txt[32] = {'\0'};

        int fd;
        if(config.GPUVendor == 1) {
            fd = open(config.temp.c_str(), O_RDONLY); // before it was zone 10
        }
        else
           fd= open(config.temp.c_str(), O_RDONLY);


        if (fd > -1) {
            read(fd, txt, 31);
            temp = std::stoi(txt );

            temp = temp/1000;

            close(fd);

//            json &tempNode =    Scene::stats.root["gpu"]["temp"];
//
//            json obj = json::object();
//            //obj[std::to_string(diffms)] = temp;
//            obj["ts"] = diffms;
//            obj["tmp"] = temp;
//            tempNode.push_back(obj);

        }
    }

//// engery value
//    {
//        std::map<std::string, std::string> power_mapping;
//
//        char txt[256] = {'\0'};
//        for(int i =0 ; i <2; i++) {
//            char buff[512]={'\0'};
//            sprintf(buff, "/sys/bus/iio/devices/iio\:device%d/energy_value", i);
//            std::string railBuffer;
//
//            int fd = open(buff, O_RDONLY);
//
//            if (fd > -1) {
//                int readByte;
//                while((readByte = read(fd,txt, 255)) > 0)
//                {
//                    railBuffer += std::string( txt, readByte);
//                }
//
//                close(fd);
//                std::vector<std::string> tokens;
//                std::regex ENERGY_PATTERN("CH([0-9]{1,2})\\(T=([0-9]+)\\)\\[([A-Z0-9_]+)\\],\\s([0-9]+)");
//                //std::list<std::tuple<std::string, std::string, std::string>> eng_mapping;
//                tokens = splitString(railBuffer);
//                for (auto const &token : tokens)
//                {
//                    // std::cout << token << std::endl;
//                    if (token != "")
//                    {
//                        std::smatch matches;
//                        if (std::regex_search(token, matches, ENERGY_PATTERN)) {
//
//                            std::string channel = matches[1].str();
//                            std::string ts = matches[2].str();
//                            std::string source = matches[3].str();
//                            std::string acc_power = matches[4].str();
//                            if( rail_mapping.find(source) !=  rail_mapping.end() ) {
//                                power_mapping[ rail_mapping[source]] = acc_power;
//                            }
//                        }
//                    }
//                }
//
//                if( power_mapping.find("GPU") != power_mapping.end())
//                gpuEng = std::stof(power_mapping["GPU"])/1000;
////                if( power_mapping.find("Camera") != power_mapping.end())
////                gpuCam = std::stof(power_mapping["Camera"])/1000000;
////                if( power_mapping.find("DDR") != power_mapping.end())
////                gpuDDR = std::stof(power_mapping["DDR"])/1000000;
////                if( power_mapping.find("Display") != power_mapping.end())
////                gpuDisplay = std::stof(power_mapping["Display"])/1000000;
//
//
////                for (auto const& x : power_mapping)
////                {
////                    json &tempNode =    Scene::stats.root["gpu"][x.first];
////                    json obj = json::object();
////                    obj[std::to_string(diffms)] = x.second;
////                    tempNode.push_back(obj);
////
////                }
//
//
//
//            }
//        }
//
//
//    }



    //STrace << "glload name:" << name() << " utilization:" << txt << " %";
     STrace << "utilization:" <<  utilization <<  " cur_freq:" << cur_freq  << " temp:" <<  temp << " fps:" <<   " frametime " <<  (fps ?  1000.0 / fps:0);
    //STrace << "thermal_zone10(deg):" << temp ;

    // CVS start  Export to Excel
     //STrace <<    utilization << "," <<  cur_freq_per  << "," <<  temp  << "," << gpuEng << "," <<  average_fps() <<  "," <<  1000.0 / average_fps();
    // CVS end


}



void Scene::gpuinit( bool showFullStats, Config& config )
{
    if(!showFullStats)
    {
        return;
    }


    ms = 0;

    {
        char txt[32] = {'\0'};

        int fd;
        if(config.GPUVendor == 1) {
           fd= open("/sys/devices/platform/1f000000.mali/min_freq", O_RDONLY);
        }
        else
          fd= open("/sys/devices/platform/34f00000.gpu0/devfreq/34f00000.gpu0/min_freq", O_RDONLY);


        if (fd > -1) {
            read(fd, txt, 31);
            min_freq = std::stoi(txt );
         //   STrace << "min_freq:" << txt;
            close(fd);

        }
    }

    {
        char txt[32] = {'\0'};
        int fd;
        if(config.GPUVendor == 1) {
            fd =open("/sys/devices/platform/1f000000.mali/max_freq", O_RDONLY);
        }
        else
            fd = open("/sys/devices/platform/34f00000.gpu0/devfreq/34f00000.gpu0/max_freq", O_RDONLY);
        if (fd > -1) {
            read(fd, txt, 31);
            max_freq = std::stoi(txt );
          //  STrace << "max_freq:" << txt;
            close(fd);

        }
    }
  // for rail
//    {
//        char txt[256] = {'\0'};
//        for(int i =0 ; i <2; i++) {
//            char buff[512]={'\0'};
//            sprintf(buff, "/sys/bus/iio/devices/iio\:device%d/available_rails", i);
//            std::string railBuffer;
//
//            int fd = open(buff, O_RDONLY);
//
//            if (fd > -1) {
//                int readByte;
//                while((readByte = read(fd,txt, 255)) > 0)
//                {
//                    railBuffer += std::string( txt, readByte);
//                }
//
//                close(fd);
//                std::vector<std::string> tokens;
//                std::regex RAIL_PATTERN("([A-Z0-9]+)\\(([A-Z0-9_]+)\\):([A-Za-z0-9()]+)");
//
//                tokens = splitString(railBuffer);
//                for (auto const &token : tokens)
//                {
//                    // std::cout << token << std::endl;
//                    if (token != "")
//                    {
//                        std::smatch matches;
//                        if (std::regex_search(token, matches, RAIL_PATTERN)) {
//
//                            std::string type = matches[2].str();
//                            std::string source =matches[3].str();
//                            rail_mapping[type] = source;
//                           // Scene::stats.root["gpu"][source]=  json::array();  // power json
//                        }
//
//                    }
//                }
//
//            }
//        }
//
//
//    }

}

Scene::Option::Option(const std::string &nam, const std::string &val, const std::string &desc,
                      const std::string &values) :
name(nam), value(val), default_value(val), description(desc), set(false)
{
    Util::split(values, ',', acceptable_values, Util::SplitModeNormal);
}



//struct Scene::Stats Scene::stats;
//void Scene::Settats(json &cnfg)
//{
//    if( cnfg.is_null() )
//    {
//        Scene::stats.root["gpu"] = json::object();
//        Scene::stats.root["gpu"]["util"]=  json::array();
//        Scene::stats.root["gpu"]["clk"]=  json::array();
//        Scene::stats.root["gpu"]["temp"]=  json::array();
//
//    }
//    else
//        Scene::stats.root = cnfg;
//
//}

Scene::Scene(Canvas &pCanvas, const string &name) :
    canvas_(pCanvas), name_(name),
    startTime_(0), lastUpdateTime_(0), currentFrame_(0),
    running_(0), duration_(0), nframes_(0)
{

    storagePath = pCanvas.storagePath;


    //statFile.load(storagePath + name+ ".json" );

    //Settats( statFile.root);


    options_["duration"] = Scene::Option("duration", "20.0",
                                         "The duration of each benchmark in seconds");

    /* FPS options */
    options_["show-stats"] = Scene::Option("show-stats", "false",
                                         "Show live stats counter",
                                         "false,true");
    options_["stats-pos"] = Scene::Option("stats-pos", "-1.0,-1.0",
                                         "The position on screen where to show stats");
    options_["stats-size"] = Scene::Option("stats-size", "0.03",
                                         "The width of each glyph for the stats");

    options_["show-power"] = Scene::Option("show-power", "false",
                                           "Show power stats counter",
                                           "false,true");
    options_["validate"] = Scene::Option("validate", "false",
                                           "validate",
                                           "false,true");

    //statsInit();
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

    //bool imgTech = false;
    //   const char* vendorChr = reinterpret_cast<const char*>(glGetString(GL_VENDOR));

    std::stringstream ss;
    ss << glGetString(GL_VENDOR) << std::endl;

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
Scene::shouldvalidate()
{
    return (options_["validate"].value == "true");
}

Scene::ValidationResult
Scene::validate( std::string &file, int &w , int &h)
{
    if( options_["validate"].value == "true" &&  validationPassed == Scene::ValidationSuccess )
    {
         file = storagePath + name_ + std::to_string(frameCount++) + "_" + std::to_string(canvas_.width())+ "_" + std::to_string(canvas_.height()) + ".rgb";
         w = canvas_.width();
         h = canvas_.height();
        std::ifstream readPd(file, std::ios::binary | std::ifstream::in);
        if (readPd.is_open()) {

            int bufSize = canvas_.width() * canvas_.height() * 4;
            char *pixels = new char[bufSize];

            canvas_.read_pixelbuf(pixels);
            std::stringstream strStream;
            strStream << readPd.rdbuf();

            int ret = strncmp(pixels, strStream.str().c_str(), bufSize - 4);

            delete[] pixels;

            strStream.clear();
            readPd.close();

            if (!ret) {
                validationPassed = Scene::ValidationSuccess;
                return validationPassed;
            }
            else {
                validationPassed = Scene::ValidationFailure;
                return validationPassed;
            }


        } else {
            canvas_.write_to_file(file);
            validationPassed = Scene::ValidationSuccess;
            return validationPassed;
        }
    }

    return validationPassed;
}

bool Scene::match()
{
//     if( options_["validate"].value == "true" &&  validationPassed == Scene::ValidationSuccess )
//    {
//        std::string file = storagePath + name_ + std::to_string(frameCount++) + "_" + std::to_string(canvas_.width())+ "_" + std::to_string(canvas_.height()) + ".rgb";
//
//        std::ifstream readPd(file, std::ios::binary | std::ifstream::in);
//        if (readPd.is_open()) {
//
//            int bufSize = canvas_.width() * canvas_.height() * 4;
//            char *pixels = new char[bufSize];
//
//            canvas_.read_pixelbuf(pixels);
//            std::stringstream strStream;
//            strStream << readPd.rdbuf();
//
//            int ret = strncmp(pixels, strStream.str().c_str(), bufSize - 4);
//
//            delete[] pixels;
//            strStream.clear();
//            readPd.close();
//
//            if (!ret)
//                return true;
//            else {
//                validationPassed = Scene::ValidationFailure;
//                return false;
//            }
//
//        } else {
//            canvas_.write_to_file(file);
//            return true;
//        }
//    }

    return true;
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

void Scene::statsInit(Config & config)
{
   // Log::info("statsInit");
}
void Scene::statsRun(bool showFullStats, Config & config)
{

    //   const char* vendorChr = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
    if(config.GPUVendor < 0) {
        std::stringstream ss;
        ss << glGetString(GL_VENDOR) << std::endl;

        std::string vendor = ss.str();
        if (vendor.find("Imag") != string::npos) {
            config.GPUVendor = 2;
        }
        else
        {
            config.GPUVendor = 1;
        }

        //gpuinit(showFullStats, config);

    }


      uint64_t curenttime= Util::get_timestamp_us();

    if( curenttime <   config.starttime + (uint64_t) config.duration*1000000) {
        if( uint64_t(curenttime - config.curenttime) > uint64_t(config.interval*1000) ) {
            gpustats(showFullStats, config);
            config.curenttime= curenttime;
        }
        else
        {
            int x = 0;
        }
    }
    else
        running_ =false;
	

}
void Scene::statsStop(){
 //   Log::info("statsStop");

    //base::cnfg::saveFile(statFile.path(),  Scene::stats.root , 4);
    //gpuinit();
}
