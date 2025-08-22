/*
 *  This calls is called when we compile code with gcc on linux
 *
 */

#include "gl-headers.h"
#include "scene.h"
#include "benchmark.h"
#include "options.h"
#include "log.h"
#include "util.h"
#include "main-loop.h"
#include "benchmark-collection.h"
#include "scene-collection.h"

#include "canvas-generic.h"

#if GPULOAD_USE_X11
#include "native-state-x11.h"
#elif GPULOAD_USE_DRM
#include "native-state-drm.h"
#elif GPULOAD_USE_MIR
#include "native-state-mir.h"
#elif GPULOAD_USE_WAYLAND
#include "native-state-wayland.h"
#elif GPULOAD_USE_DISPMANX
#include "native-state-dispmanx.h"
#elif GPULOAD_USE_WIN32
#include "native-state-win32.h"
#endif

#if GPULOAD_USE_EGL
#include "gl-state-egl.h"
#elif GPULOAD_USE_GLX
#include "gl-state-glx.h"
#elif GPULOAD_USE_WGL
#include "gl-state-wgl.h"
#endif

using std::vector;
using std::map;
using std::string;

static void
list_scenes()
{
    const map<string, Scene *> &scenes = Benchmark::scenes();

    for (map<string, Scene *>::const_iterator scene_iter = scenes.begin();
         scene_iter != scenes.end();
         scene_iter++)
    {
        Scene *scene = scene_iter->second;
        if (scene->name().empty())
            continue;
        Log::info("[Scene] %s\n", scene->name().c_str());

        const map<string, Scene::Option> &options = scene->options();

        for (map<string, Scene::Option>::const_iterator opt_iter = options.begin();
             opt_iter != options.end();
             opt_iter++)
        {
            const Scene::Option &opt = opt_iter->second;
            Log::info("  [Option] %s\n"
                      "    Description  : %s\n"
                      "    Default Value: %s\n",
                      opt.name.c_str(),
                      opt.description.c_str(),
                      opt.default_value.c_str());

            /* Display list of acceptable values (if defined) */
            if (!opt.acceptable_values.empty()) {
                Log::info("    Acceptable Values: ");
                for (vector<string>::const_iterator val_iter = opt.acceptable_values.begin();
                     val_iter != opt.acceptable_values.end();
                     val_iter++)
                {
                    std::string format_value(Log::continuation_prefix + "%s");
                    if (val_iter + 1 != opt.acceptable_values.end())
                        format_value += ",";
                    else
                        format_value += "\n";
                    Log::info(format_value.c_str(), val_iter->c_str());
                }
            }
        }
    }
}

void
do_benchmark(Canvas &canvas)
{
    BenchmarkCollection benchmark_collection;
    MainLoop *loop;

    benchmark_collection.populate_from_options();
    
    if (benchmark_collection.needs_decoration())
        loop = new MainLoopDecoration(canvas, benchmark_collection.benchmarks());
    else
        loop = new MainLoop(canvas, benchmark_collection.benchmarks());

    while (loop->step());

    Log::info("=======================================================\n");
    Log::info("                                  gpuload Score: %u \n", loop->score());
    Log::info("=======================================================\n");

    delete loop;
}

void
do_validation(Canvas &canvas)
{
    BenchmarkCollection benchmark_collection;

    benchmark_collection.populate_from_options();

    MainLoopValidation loop(canvas, benchmark_collection.benchmarks());

    while (loop.step());
}

int
main(int argc, char *argv[])
{
    if (!Options::parse_args(argc, argv))
        return 1;

    /* Initialize Log class */
    Log::init(Util::appname_from_path(argv[0]), Options::show_debug);

    if (Options::show_help) {
        Options::print_help();
        return 0;
    }

    if (Options::show_version) {
        printf("%s\n", GPULOAD_VERSION);
        return 0;
    }

    /* Force 800x600 output for validation */
    if (Options::validate &&
        Options::size != std::pair<int,int>(800, 600))
    {
        Log::info("Ignoring custom size %dx%d for validation. Using 800x600.\n",
                  Options::size.first, Options::size.second);
        Options::size = std::pair<int,int>(800, 600);
    }

    // Create the canvas
#if GPULOAD_USE_X11
    NativeStateX11 native_state;
#elif GPULOAD_USE_DRM
    NativeStateDRM native_state;
#elif GPULOAD_USE_MIR
    NativeStateMir native_state;
#elif GPULOAD_USE_WAYLAND
    NativeStateWayland native_state;
#elif GPULOAD_USE_DISPMANX
    NativeStateDispmanx native_state;
#elif GPULOAD_USE_WIN32
    NativeStateWin32 native_state;
#endif

#if GPULOAD_USE_EGL
    GLStateEGL gl_state;
#elif GPULOAD_USE_GLX
    GLStateGLX gl_state;
#elif GPULOAD_USE_WGL
    GLStateWGL gl_state;
#endif

    CanvasGeneric canvas(native_state, gl_state, Options::size.first, Options::size.second);

    canvas.offscreen(Options::offscreen);

    canvas.visual_config(Options::visual_config);

    // Register the scenes, so they can be looked up by name
    SceneCollection scenes(canvas);
    scenes.register_scenes();

    if (Options::list_scenes) {
        list_scenes();
        return 0;
    }

    if (!canvas.init()) {
        Log::error("%s: Could not initialize canvas\n", __FUNCTION__);
        return 1;
    }

    Log::info("=======================================================\n");
    Log::info("    gpuload %s\n", GPULOAD_VERSION);
    Log::info("=======================================================\n");
    canvas.print_info();
    Log::info("=======================================================\n");

    canvas.visible(true);

    if (Options::validate)
        do_validation(canvas);
    else
        do_benchmark(canvas);

    return 0;
}
