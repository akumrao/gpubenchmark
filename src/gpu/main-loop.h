/*
 * Scene loops and steps are controlled from this class
 *
*/
#ifndef GPULOAD_MAIN_LOOP_H_
#define GPULOAD_MAIN_LOOP_H_

#include "canvas.h"
#include "benchmark.h"
#include "text-renderer.h"
#include "vec.h"
#include "default-benchmarks.h"

#include <vector>

/**
 * Main loop for benchmarking.
 */
class MainLoop
{
public:
    MainLoop(Canvas &canvas, const std::vector<Benchmark *> &benchmarks,  Config &config);

    virtual ~MainLoop() {}

    /**
     * Resets the main loop.
     *
     * You need to call reset() if the loop has finished and
     * you need to run it again.
     */
    void reset();

    /**
     * Gets the current total benchmarking score.
     */
    unsigned int score();

    /**
     * Perform the next main loop step.
     *
     * @returns whether the loop has finished
     */
    bool step(int battery, bool isPowSaveMode,  bool isLowPowerStandbyEnabled, bool isSustainedPerformanceModeSupported);

    /**
     * Overridable method for drawing the canvas contents.
     */
    virtual void draw();

    /**
     * Overridable method for pre scene-setup customizations.
     */
    virtual void before_scene_setup() {}

    /**
     * Overridable method for post scene-setup customizations.
     */
    virtual void after_scene_setup(int utilization, int clock , int temp) {}

    /**
     * Overridable method for logging scene info.
     */
    virtual void log_scene_info();

    /**
     * Overridable method for logging scene result.
     */
    virtual void log_scene_result(std::string &file, int &w , int &h);

protected:
    enum SceneSetupStatus {
        SceneSetupStatusUnknown,
        SceneSetupStatusSuccess,
        SceneSetupStatusFailure,
        SceneSetupStatusUnsupported
    };
    void next_benchmark();
    Canvas &canvas_;
    Scene *scene_;
    const std::vector<Benchmark *> &benchmarks_;
    unsigned int score_;
    unsigned int benchmarks_run_;
    SceneSetupStatus scene_setup_status_;

    std::vector<Benchmark *>::const_iterator bench_iter_;

public:
    Config & config;


    struct stats
    {
        int cur_freq;
        int utilization;
        int temp;
        int result{-1};
    };

    std::vector<stRgbInf> clipstorage;

    //std::map<std::string,  std::map<int, stats >> mapStats;
    void analyzeResult();
    void printResult();
    int variation{20};
};

/**
 * Main loop for benchmarking with decorations (eg FPS, demo)
 */
class MainLoopDecoration : public MainLoop
{
public:
    MainLoopDecoration(Canvas &canvas, const std::vector<Benchmark *> &benchmarks, Config &config);
    virtual ~MainLoopDecoration();

    virtual void draw();
    virtual void before_scene_setup();
    virtual void after_scene_setup(int utilization, int clock , int temp);


protected:
    void fps_renderer_update_text(unsigned int fps, int utilization, int clock , int temp);
    LibMatrix::vec2 vec2_from_pos_string(const std::string &s);

    bool show_stats;
    bool show_title_;
    TextRenderer *fps_renderer_;
    TextRenderer *title_renderer_;
    unsigned int last_fps_;
    uint64_t fps_timestamp_;
};

/**
 * Main loop for validation.
 */
class MainLoopValidation : public MainLoop
{
public:
    MainLoopValidation(Canvas &canvas, const std::vector<Benchmark *> &benchmarks, Config &config);

    virtual void draw();
    virtual void log_scene_result(std::string &file, int &w , int &h);
};

#endif /* GPULOAD_MAIN_LOOP_H_ */
