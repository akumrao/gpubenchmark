/*
 * Scene loops and steps are controlled from this class
 *
*/

#include "options.h"
#include "main-loop.h"
#include "util.h"
#include "log.h"

#include <string>
#include <sstream>

/************
 * MainLoop *
 ************/

MainLoop::MainLoop(Canvas &canvas, const std::vector<Benchmark *> &benchmarks, Config &config) :
    canvas_(canvas), benchmarks_(benchmarks), config(config)
{
    reset();
}


void
MainLoop::reset()
{
    scene_ = 0;
    scene_setup_status_ = SceneSetupStatusUnknown;
    score_ = 0;
    benchmarks_run_ = 0;
    bench_iter_ = benchmarks_.begin();

    clipstorage.clear();
}

unsigned int
MainLoop::score()
{
    if (benchmarks_run_)
        return score_ / benchmarks_run_;
    else
        return score_;
}

bool
MainLoop::step( int battery, bool isPowSaveMode,  bool isLowPowerStandbyEnabled, bool isSustainedPerformanceModeSupported)
{
    /* Find the next normal scene */
    if (!scene_) {
        /* Find a normal scene */
        while (bench_iter_ != benchmarks_.end())
        {
            scene_ = &(*bench_iter_)->scene();
            scene_->battery = battery;
            scene_->isPowSaveMode = isPowSaveMode;

            if(scene_->name() == "buffer" )
            {
                scene_->clipstorage=clipstorage;
            }
            //scene_->isLowPowerStandbyEnabled = isLowPowerStandbyEnabled;
            //scene_->isSustainedPerformanceModeSupported = isSustainedPerformanceModeSupported;

            /* 
             * Scenes with empty names are option-setting scenes.
             * Just set them up and continue with the search.
             */
            if (scene_->name().empty())
            {
                (*bench_iter_)->setup_scene();
               // scene_->statsInit();
            }
            else
                break;

            next_benchmark();
        }

        /* If we have found a valid scene, set it up */
        if (bench_iter_ != benchmarks_.end()) {
            before_scene_setup();
            if (!Options::reuse_context)
                canvas_.reset();
            scene_ = &(*bench_iter_)->setup_scene();
            if (!scene_->running()) {
                if (!scene_->supported(false))
                    scene_setup_status_ = SceneSetupStatusUnsupported;
                else
                    scene_setup_status_ = SceneSetupStatusFailure;
            }
            else {
                scene_setup_status_ = SceneSetupStatusSuccess;
            }


            int cur_freq = scene_->cur_freq_per;  // arvind
            int utilization = scene_->utilization;
            int temp = scene_->temp;
            after_scene_setup( utilization,  cur_freq ,  temp );


            log_scene_info();
        }
        else {
            /* ... otherwise we are done */
            return false;
        }
    }

    bool should_quit = canvas_.should_quit();

    if (scene_ ->running() && !should_quit)
        draw();

    /*
     * Need to recheck whether the scene is still running, because code
     * in draw() may have changed the state.
     */
    if (!scene_->running() || should_quit) {
        if (scene_setup_status_ == SceneSetupStatusSuccess) {
            score_ += scene_->average_fps();
            benchmarks_run_++;
        }

        stRgbInf rgbInf;
        log_scene_result(rgbInf.file, rgbInf.w, rgbInf.h);
        clipstorage.push_back(rgbInf);

        scene_->statsStop();
        printResult();
        (*bench_iter_)->teardown_scene();
        scene_ = 0;
        next_benchmark();
    }

    return !should_quit;
}

void
MainLoop::draw()
{
    canvas_.clear();


    scene_->statsRun(false, config);

    scene_->draw();

//    Canvas::Pixel p =  canvas_.read_pixel( 200, 200);
//    uint32_t tmp =    p.to_le32();
//
//
//    if( tmp > 0)
//    {
//        static const std::string format(Log::continuation_prefix + " arvind: %d\n");
//
//
//        Log::info(format.c_str(), tmp);
//
//        tmp = 0;
//    }

    scene_ ->match();

    scene_->update();

    analyzeResult();

    canvas_.update();
}

void MainLoop::analyzeResult() {
    int cur_freq = scene_->cur_freq_per;  // arvind
    int utilization = scene_->utilization;
    int temp = scene_->temp;

    if( !cur_freq || !utilization || !temp )
    {
        return;
    }

    std::string name = scene_->name() ;
//
//    if (mapStats[name].result == 1)
//    {
//        int pre_cur_freq= mapStats[name].cur_freq;
//        int pre_cur_util=mapStats[name].utilization;
//        int pre_cur_temp=mapStats[name].temp;
//
//        if (abs( (pre_cur_freq - cur_freq)) > mapStats[name].variation)
//        {
//            mapStats[name].result = 0;
//        }
//
//        if (abs(pre_cur_util - utilization) >    mapStats[name].variation)
//        {
//            mapStats[name].result = 0;
//        }
//
//
//        if (abs( pre_cur_temp - temp) > mapStats[name].variation)
//        {
//            mapStats[scene_->name()].result = 0;
//        }
//    } else
//      if (mapStats[name][benchmarks_run_].result == -1) {
//        mapStats[name][benchmarks_run_].result = 1;
//        mapStats[name][benchmarks_run_].cur_freq = cur_freq;
//        mapStats[name][benchmarks_run_].utilization = utilization;
//        mapStats[name][benchmarks_run_].temp = temp;
//        return;
//    }
//    else
//    {
//        mapStats[name][benchmarks_run_].cur_freq = (mapStats[name][benchmarks_run_].cur_freq + cur_freq)/2;
//        mapStats[name][benchmarks_run_].utilization = (mapStats[name][benchmarks_run_].utilization + utilization)/2;
//        mapStats[name][benchmarks_run_].temp = (mapStats[name][benchmarks_run_].temp + temp)/2;
//    }


}

void MainLoop::printResult()
{
//    for (auto i = mapStats.begin(); i != mapStats.end(); i++)
//    {
//        int cur_freq = 0;
//        int utilization = 0;
//        int temp = 0;
//        for (auto j = i->second.begin(); j != i->second.end(); j++) {
//            if(!cur_freq)
//            cur_freq = j->second.cur_freq;
//            else
//            {
//                if ( abs( cur_freq - j->second.cur_freq) > variation)
//                {
//                    Log::info("gpuload Test %s\n", "Failed");
//                    return;
//                }
//            }
//
//            if(!utilization)
//                utilization = j->second.utilization;
//            else
//            {
//                if ( abs( utilization - j->second.utilization) > variation)
//                {
//                    Log::info("gpuload Test %s\n", "Failed");
//                    return;
//                }
//            }
//
//            if(!temp)
//                temp = j->second.temp;
//            else
//            {
//                if ( abs( temp - j->second.temp) > variation)
//                {
//                    Log::info("gpuload Test %s\n", "Failed");
//                    return;
//                }
//            }
//
//
////            if (i->second.result == 0) {
////                Log::info("gpuload Test %s\n", "Failed");
////                return;
////            }
//        }
//       // cout << i->first << " \t\t\t" << i->second << endl;
//    }

    Log::info("gpuload Test %s\n", "Passed");

}

void
MainLoop::log_scene_info()
{
    Log::info("%s", scene_->info_string().c_str());
    Log::flush();
}

void
MainLoop::log_scene_result(std::string &file, int &w , int &h)
{
    static const std::string format_fps(Log::continuation_prefix +
                                        " FPS: %u FrameTime: %.3f ms\n");
    static const std::string format_unsupported(Log::continuation_prefix +
                                                " Unsupported\n");
    static const std::string format_fail(Log::continuation_prefix +
                                         " Set up failed\n");

    if (scene_setup_status_ == SceneSetupStatusSuccess) {
        Log::info(format_fps.c_str(), scene_->average_fps(),
                                      1000.0 / scene_->average_fps());
    }
    else if (scene_setup_status_ == SceneSetupStatusUnsupported) {
        Log::info(format_unsupported.c_str());
    }
    else {
        Log::info(format_fail.c_str());
    }
}

void
MainLoop::next_benchmark()
{
    bench_iter_++;
    if (bench_iter_ == benchmarks_.end() && Options::run_forever)
        bench_iter_ = benchmarks_.begin();
}

/**********************
 * MainLoopDecoration *
 **********************/

MainLoopDecoration::MainLoopDecoration(Canvas &canvas, const std::vector<Benchmark *> &benchmarks, Config &config) :
    MainLoop(canvas, benchmarks, config), show_stats(false), show_title_(false),
    fps_renderer_(0), title_renderer_(0), last_fps_(0)
{

}

MainLoopDecoration::~MainLoopDecoration()
{
    delete fps_renderer_;
    fps_renderer_ = 0;
    delete title_renderer_;
    title_renderer_ = 0;
}

void
MainLoopDecoration::draw()
{
    static const unsigned int fps_interval = 500000;

    canvas_.clear();
    //scene_->statsRun(show_stats, config);
    scene_->draw();

    scene_ ->match();

    scene_->update();

    if (show_stats) {
        uint64_t now = Util::get_timestamp_us();
        if (now - fps_timestamp_ >= fps_interval) {
            last_fps_ = scene_->average_fps();

            int cur_freq = scene_->cur_freq;  // arvind
            int utilization = scene_->utilization;
            int temp = scene_->temp;
            after_scene_setup( utilization,  cur_freq ,  temp );

            //fps_renderer_update_text(last_fps_);
            fps_timestamp_ = now;
        }
        fps_renderer_->render();
    }

    if (show_title_)
        title_renderer_->render();

    canvas_.update();
}

void
MainLoopDecoration::before_scene_setup()
{
    delete fps_renderer_;
    fps_renderer_ = 0;
    delete title_renderer_;
    title_renderer_ = 0;
}

void
MainLoopDecoration::after_scene_setup(int utilization, int clock , int temp)
{
    const Scene::Option &show_fps_option(scene_->options().find("show-stats")->second);
    const Scene::Option &title_option(scene_->options().find("title")->second);
    show_stats = show_fps_option.value == "true";
    show_title_ = !title_option.value.empty();

    if (show_stats) {
        const Scene::Option &fps_pos_option(scene_->options().find("stats-pos")->second);
        const Scene::Option &fps_size_option(scene_->options().find("stats-size")->second);

        fps_renderer_ = new TextRenderer(canvas_);
        fps_renderer_->position(vec2_from_pos_string(fps_pos_option.value));
        fps_renderer_->size(Util::fromString<float>(fps_size_option.value));
        fps_renderer_update_text(last_fps_,utilization, clock , temp);
        fps_timestamp_ = Util::get_timestamp_us();




    }

    if (show_title_) {
        const Scene::Option &title_pos_option(scene_->options().find("title-pos")->second);
        const Scene::Option &title_size_option(scene_->options().find("title-size")->second);
        title_renderer_ = new TextRenderer(canvas_);
        title_renderer_->position(vec2_from_pos_string(title_pos_option.value));
        title_renderer_->size(Util::fromString<float>(title_size_option.value));

        if (title_option.value == "#info#")
            title_renderer_->text(scene_->info_string());
        else if (title_option.value == "#name#")
            title_renderer_->text(scene_->name());
        else if (title_option.value == "#r2d2#")
            title_renderer_->text("arvind you are my helo");
        else
            title_renderer_->text(title_option.value);
    }
}

void
MainLoopDecoration::fps_renderer_update_text(unsigned int fps, int utilization, int clock , int temp)
{

    std::stringstream ss;
    const Scene::Option &power_stats(scene_->options().find("show-power")->second);
    if(power_stats.value == "true")
    {
        ss << "gpu: " << scene_->gpuEng <<  " ddr: " <<  scene_->gpuDDR  <<  " cam: " <<  scene_->gpuCam <<  " disp: " <<  scene_->gpuDisplay;
    }
    else
    ss << "FPS: " <<  fps << " usg: " << utilization << " clk: " << clock <<  " temp: " << temp << " bat:"   << scene_->battery << " lpm: "  <<  scene_->isPowSaveMode << " eng: " <<  scene_->gpuEng   ;
    fps_renderer_->text(ss.str());
}

LibMatrix::vec2
MainLoopDecoration::vec2_from_pos_string(const std::string &s)
{
    LibMatrix::vec2 v(0.0, 0.0);
    std::vector<std::string> elems;
    Util::split(s, ',', elems, Util::SplitModeNormal);

    if (elems.size() > 0)
        v.x(Util::fromString<float>(elems[0]));

    if (elems.size() > 1)
        v.y(Util::fromString<float>(elems[1]));

    return v;
}

/**********************
 * MainLoopValidation *
 **********************/

MainLoopValidation::MainLoopValidation(Canvas &canvas, const std::vector<Benchmark *> &benchmarks, Config &config) :
        MainLoop(canvas, benchmarks, config )
{
}

void
MainLoopValidation::draw()
{
    /* Draw only the first frame of the scene and stop */
    canvas_.clear();

    scene_->draw();

    canvas_.update();
    analyzeResult();
    scene_->running(false);
}

void
MainLoopValidation::log_scene_result(std::string &file, int &w , int &h)
{
    static const std::string format(Log::continuation_prefix + " Validation: %s\n");
    std::string result;

    switch(scene_->validate(file, w, h)) {
        case Scene::ValidationSuccess:
            result = "Success";
            break;
        case Scene::ValidationFailure:
            result = "Failure";
            break;
        case Scene::ValidationUnknown:
            result = "Unknown";
            break;
        default:
            break;
    }

    Log::info(format.c_str(), result.c_str());
}
