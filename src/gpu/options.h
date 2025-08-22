/*
 *  Scene options
 *
 */

#ifndef OPTIONS_H_
#define OPTIONS_H_

#include <string>
#include <vector>
#include "gl-visual-config.h"

struct Options {
    enum FrameEnd {
        FrameEndDefault,
        FrameEndNone,
        FrameEndSwap,
        FrameEndFinish,
        FrameEndReadPixels
    };

    enum SwapMode {
        SwapModeDefault,
        SwapModeImmediate,
        SwapModeMailbox,
        SwapModeFIFO,
    };

    static bool parse_args(int argc, char **argv);
    static void print_help();

    static std::vector<std::string> benchmarks;
    static std::vector<std::string> benchmark_files;
    static bool validate;
    static std::string data_path;
    static FrameEnd frame_end;
    static SwapMode swap_mode;
    static std::pair<int,int> size;
    static bool list_scenes;
    static bool show_all_options;
    static bool show_debug;
    static bool show_version;
    static bool show_help;
    static bool reuse_context;
    static bool run_forever;
    static bool annotate;
    static bool offscreen;
    static GLVisualConfig visual_config;
};

#endif /* OPTIONS_H_ */
