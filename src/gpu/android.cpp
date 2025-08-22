/*
 * Class for JNI
 *  INIT ->  RESIZE  ->LOAD  ->RENDER  ->DONE  & SCROECONFIG & INFO
 */

#include <assert.h>
#include <jni.h>
#include <vector>
#include <string>
#include <fstream>
#include "canvas-android.h"
#include "benchmark.h"
#include "options.h"
#include "log.h"
#include "util.h"
#include "main-loop.h"
#include "benchmark-collection.h"
#include "scene-collection.h"

//#include "base/process.h"
#include "base/logger.h"

//#include "process.h"
using namespace base;



static Canvas *g_canvas;
static MainLoop *g_loop;
static BenchmarkCollection *g_benchmark_collection;
static SceneCollection *g_scene_collection;
static std::ostream *g_log_extra;
static std::string g_storagePath;

//extern "C" {
//int mainfn(int argc, char **argv, char **envp);
//}


//
//class CPUThread : public Thread {
//public:
//
//    CPUThread(std::string host)  {
//
//        STrace << "Stats at Start of APP";
//
//        //--cpu 8
//
//
//       // gpuinit();
//
//       // gpustats();
//
//    }
//    // virtual ~Thread2(void);
//
//    void run() {
//
//        STrace << "Stats at load time";
//
//        while(!stopped())
//        {
//
////            char *argv1[5];
////            char *envp[1] ;
////
////            argv1[0]="stress-ng";
////            argv1[1]="--cpu";
////            argv1[2]="8";
////            argv1[3]="--temp-path";
////            argv1[4]="/data/local/tmp/";
////
////            envp[0]="ng-stress";
////
////            mainfn(3, argv1, 0);
//           // sleep(1);
//        }
//
//
//    }
//
//
//};
//
//#define CPULOAD 0
//
//CPUThread *cpuThread[CPULOAD];



class MainLoopAndroid : public MainLoop
{
public:
    MainLoopAndroid(Canvas &canvas, const std::vector<Benchmark *> &benchmarks, Config &config) :
        MainLoop(canvas, benchmarks,config ) {}

    virtual void log_scene_info() {}

    void log_scene_validate( std::string &file, int &w , int &h)
    {
            if(scene_->shouldvalidate() ) {
                static const std::string format(Log::continuation_prefix + " Validation: %s\n");
                std::string result;

                switch (scene_->validate(file, w, h)) {
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
   }

    virtual void log_scene_result(std::string &file, int &w , int &h)
    {
        log_scene_validate(file, w, h);

        if (scene_setup_status_ == SceneSetupStatusSuccess) {

//            Log::info("frq: %d, utilization: %d, temp(degcel) %d \n",
//                      scene_->cur_freq,
//                      scene_->utilization,
//                       scene_->temp);

            Log::info("%s FPS: %u FrameTime: %.3f ms\n",
                      scene_->info_string().c_str(),
                      scene_->average_fps(),
                      1000.0 / scene_->average_fps());

        }
        else if (scene_setup_status_ == SceneSetupStatusUnsupported) {
            Log::info("%s Unsupported\n",
                      scene_->info_string().c_str());
        }
        else {
            Log::info("%s Set up failed\n",
                      scene_->info_string().c_str());
        }
    }
};

class MainLoopDecorationAndroid : public MainLoopDecoration
{
public:
    MainLoopDecorationAndroid(Canvas &canvas, const std::vector<Benchmark *> &benchmarks , Config &config) :
        MainLoopDecoration(canvas, benchmarks, config) {}

    virtual void log_scene_info() {}
    void log_scene_validate(std::string &file, int &w , int &h)
    {
        if(scene_->shouldvalidate() ) {
            static const std::string format(Log::continuation_prefix + " Validation: %s\n");
            std::string result;

            switch (scene_->validate(file,w,h)) {
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
    }
    virtual void log_scene_result(std::string &file, int &w , int &h)
    {
        log_scene_validate(file, w, h);

        if (scene_setup_status_ == SceneSetupStatusSuccess) {
            Log::info("%s FPS: %u FrameTime: %.3f ms\n",
                      scene_->info_string().c_str(),
                      scene_->average_fps(),
                      1000.0 / scene_->average_fps());
        }
        else if (scene_setup_status_ == SceneSetupStatusUnsupported) {
            Log::info("%s Unsupported\n",
                      scene_->info_string().c_str());
        }
        else {
            Log::info("%s Set up failed\n",
                      scene_->info_string().c_str());
        }
    }
};

/** 
 * Converts an std::vector containing arguments to argc,argv.
 */
static void
arg_vector_to_argv(const std::vector<std::string> &arguments, int &argc, char **&argv)
{
    argc = arguments.size() + 1;
    argv = new char* [argc];
    argv[0] = strdup("gpuload");

    for (unsigned int i = 0; i < arguments.size(); i++)
        argv[i + 1] = strdup(arguments[i].c_str());
}

/** 
 * Populates the command line arguments from the arguments file.
 * 
 * @param argc the number of arguments
 * @param argv the argument array
 */
static void
get_args_from_file(const std::string &arguments_file, int &argc, char **&argv)
{
    std::vector<std::string> arguments;
    std::ifstream ifs(arguments_file.c_str());

    if (!ifs.fail()) {
        std::string line;
        while (getline(ifs, line)) {
            if (!line.empty())
                Util::split(line, ' ', arguments, Util::SplitModeQuoted);
        }
    }

    arg_vector_to_argv(arguments, argc, argv);
}

/** 
 * Populates the command line arguments from the arguments file.
 * 
 * @param argc the number of arguments
 * @param argv the argument array
 */
static void
get_args_from_string(const std::string &args_str, int &argc, char **&argv)
{
    std::vector<std::string> arguments;
    Util::split(args_str, ' ', arguments, Util::SplitModeQuoted);

    arg_vector_to_argv(arguments, argc, argv);
}

/** 
 * Releases the command line arguments.
 * 
 * @param argc the number of arguments
 * @param argv the argument array
 */
static void
release_args(int argc, char **argv)
{
    for (int i = 0; i < argc; i++)
        free(argv[i]);

    delete[] argv;
}

/** 
 * Converts a GLVisualConfig Java object to a GLVisualConfig C++ object.
 * 
 * @param env the JNIEnv
 * @param jvc the Java VisualConfig object to convert
 * @param vc the C++ VisualConfig object to fill
 */
static void
gl_visual_config_from_jobject(JNIEnv *env, jobject jvc, GLVisualConfig &vc)
{
    jclass cls = env->GetObjectClass(jvc);
    jfieldID fid;

    fid = env->GetFieldID(cls, "red", "I");
    vc.red = env->GetIntField(jvc, fid);

    fid = env->GetFieldID(cls, "green", "I");
    vc.green = env->GetIntField(jvc, fid);
    
    fid = env->GetFieldID(cls, "blue", "I");
    vc.blue = env->GetIntField(jvc, fid);

    fid = env->GetFieldID(cls, "alpha", "I");
    vc.alpha = env->GetIntField(jvc, fid);

    fid = env->GetFieldID(cls, "depth", "I");
    vc.depth = env->GetIntField(jvc, fid);

    fid = env->GetFieldID(cls, "buffer", "I");
    vc.buffer = env->GetIntField(jvc, fid);
}

/** 
 * Creates a SceneInfo Java object from a Scene.
 * 
 * @param env the JNIEnv
 */
static jobject
scene_info_from_scene(JNIEnv *env, Scene &scene)
{
    jclass cls = env->FindClass("org/gpu/glload/SceneInfo");
    jmethodID constructor = env->GetMethodID(cls, "<init>", "(Ljava/lang/String;)V");
    jmethodID add_option = env->GetMethodID(cls, "addOption", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;[Ljava/lang/String;)V");

    /* Create the SceneInfo object */
    jstring name = env->NewStringUTF(scene.name().c_str());
    jobject scene_info = env->NewObject(cls, constructor, name);

    const std::map<std::string, Scene::Option> &options = scene.options();

    /* Add options to the SceneInfo object */
    for (std::map<std::string, Scene::Option>::const_iterator opt_iter = options.begin();
         opt_iter != options.end();
         opt_iter++)
    {
        const Scene::Option &opt = opt_iter->second;
        jstring opt_name = env->NewStringUTF(opt.name.c_str());
        jstring opt_description = env->NewStringUTF(opt.description.c_str());
        jstring opt_default_value = env->NewStringUTF(opt.default_value.c_str());

        /* Create and populate the acceptable values array */
        jclass string_cls = env->FindClass("java/lang/String");
        jobjectArray opt_acceptable_values = env->NewObjectArray(opt.acceptable_values.size(),
                                                                 string_cls, 0);
        
        for (size_t i = 0; i < opt.acceptable_values.size(); i++) {
            jstring opt_value = env->NewStringUTF(opt.acceptable_values[i].c_str());
            env->SetObjectArrayElement(opt_acceptable_values, i, opt_value);
            env->DeleteLocalRef(opt_value);
        }

        env->CallVoidMethod(scene_info, add_option,
                            opt_name,
                            opt_description,
                            opt_default_value,
                            opt_acceptable_values);

        env->DeleteLocalRef(opt_name);
        env->DeleteLocalRef(opt_description);
        env->DeleteLocalRef(opt_default_value);
        env->DeleteLocalRef(opt_acceptable_values);
    }

    return scene_info;
}

class DummyCanvas : public Canvas {
public:
    DummyCanvas() : Canvas(0, 0) {}
};

void
Java_org_gpu_glload_native_init(JNIEnv* env, jclass clazz,
                                    jobject asset_manager,
                                    jstring args,
                                    jstring log_file)
{
    static_cast<void>(clazz);
    static const std::string arguments_file("/data/gpuload/args");
    int argc = 0;
    char **argv = 0;

    /* Load arguments from argument string or arguments file and parse them */
    if (args) {
        if (env->GetStringUTFLength(args) > 0) {
            const char *args_c_str = env->GetStringUTFChars(args, 0);
            if (args_c_str) {
                get_args_from_string(std::string(args_c_str), argc, argv);
                env->ReleaseStringUTFChars(args, args_c_str);
            }
        }
    }
    else {
        get_args_from_file(arguments_file, argc, argv);
    }

    Options::parse_args(argc, argv);
    release_args(argc, argv);

    /* Get the log file path and open the log file */
    const char *log_file_c_str = env->GetStringUTFChars(log_file, 0);
    if (log_file_c_str) {
        g_log_extra = new std::ofstream(log_file_c_str, std::ios::binary);
        env->ReleaseStringUTFChars(log_file, log_file_c_str);
    }

    /* Force reuse of EGL/GL context */
    Options::reuse_context = true;

    Log::init("gpuload", Options::show_debug, g_log_extra);
    Util::android_set_asset_manager(AAssetManager_fromJava(env, asset_manager));

    if(g_storagePath.empty()) {
        g_storagePath = "/storage/emulated/0/Android/data/org.gpu.glload.debug/files/lists/";
        Logger::instance().add(new RotatingFileChannel("gpu_kpi", g_storagePath, Level::Trace, "log", 10000));
    }

    g_canvas = new CanvasAndroid(100, 100);
    g_canvas->storagePath = g_storagePath;
    g_canvas->init();

    Log::info("gpuload %s\n", GPULOAD_VERSION);
    g_canvas->print_info();

    /* Add and register scenes */
    g_scene_collection = new SceneCollection(*g_canvas);
    g_scene_collection->register_scenes();

    g_benchmark_collection = new BenchmarkCollection();
    g_benchmark_collection->populate_from_options(g_storagePath);

    if (g_benchmark_collection->needs_decoration()) {
        g_loop = new MainLoopDecorationAndroid(*g_canvas,
                                               g_benchmark_collection->benchmarks(),g_benchmark_collection->config);
    }
    else {
        g_loop = new MainLoopAndroid(*g_canvas,
                                     g_benchmark_collection->benchmarks() ,g_benchmark_collection->config );
    }
}

void
Java_org_gpu_glload_native_resize(JNIEnv* env,
                                      jclass clazz,
                                      jint w,
                                      jint h)
{
    static_cast<void>(env);
    static_cast<void>(clazz);

    Log::debug("Resizing to %d x %d\n", w, h);
    g_canvas->resize(w, h);
}

void
Java_org_gpu_glload_native_done(JNIEnv* env)
{
    static_cast<void>(env);

    delete g_loop;
    delete g_benchmark_collection;
    delete g_scene_collection;
    delete g_canvas;
    delete g_log_extra;
}

jboolean
Java_org_gpu_glload_native_render(JNIEnv* env, jclass clazz, jint battery, jboolean isPowSaveMode,  jboolean isLowPowerStandbyEnabled, jboolean isSustainedPerformanceModeSupported)
{
    static_cast<void>(env);
    int tes = (int)battery;
    if (!g_loop->step(battery, isPowSaveMode, isLowPowerStandbyEnabled, isSustainedPerformanceModeSupported)) {
        Log::info("GLload Score: %u\n", g_loop->score());
        return false;
    }

    return true;
}

jint
Java_org_gpu_glload_native_scoreConfig(JNIEnv* env, jclass clazz,
                                           jobject jvc, jobject jtarget)
{
    static_cast<void>(clazz);

    GLVisualConfig vc;
    GLVisualConfig target;

    gl_visual_config_from_jobject(env, jvc, vc);
    gl_visual_config_from_jobject(env, jtarget, target);

    return vc.match_score(target);
}

jobjectArray
Java_org_gpu_glload_native_getSceneInfo(JNIEnv* env, jclass clazz,
                                            jobject asset_manager,  jstring storagePath )
{

/*
   if( CPULOAD)
   {
	  for( int x =0 ; x < CPULOAD ; ++x) {
	   if (!cpuThread[x])
		 cpuThread[x] = new CPUThread("host");
	     else
		 return JNI_FALSE;

	   cpuThread[x]->start();

         }
    }

*/
    static_cast<void>(clazz);

    Util::android_set_asset_manager(AAssetManager_fromJava(env, asset_manager));

    /* Get the log file path and open the log file */
    const char *log_file_c_str = env->GetStringUTFChars(storagePath, 0);
    std::string spath;
    if (log_file_c_str) {
        spath = std::string(log_file_c_str);
        Logger::instance().add(new RotatingFileChannel("gpu_kpi", spath, Level::Trace, "log", 10000));
        env->ReleaseStringUTFChars(storagePath, log_file_c_str);
    }


    DummyCanvas canvas;
    canvas.storagePath = spath + "/";
    g_storagePath =  canvas.storagePath;
    SceneCollection sc(canvas);
    const std::vector<Scene*>& scenes = sc.get();
    std::vector<jobject> si_vector;

    /* Create SceneInfo instances for all the scenes */
    for (std::vector<Scene*>::const_iterator iter = scenes.begin();
         iter != scenes.end();
         iter++)
    {
        jobject si = scene_info_from_scene(env, **iter);
        si_vector.push_back(si);
    }

    /* Create a SceneInfo[] array */
    jclass si_cls = env->FindClass("org/gpu/glload/SceneInfo");
    jobjectArray si_array = env->NewObjectArray(si_vector.size(), si_cls, 0);
    
    /* Populate the SceneInfo[] array */
    for (size_t i = 0; i < si_vector.size(); i++)
        env->SetObjectArrayElement(si_array, i, si_vector[i]);

    return si_array;
}

static JNINativeMethod gpuload_native_methods[] = {
    {
        "init",
        "(Landroid/content/res/AssetManager;Ljava/lang/String;Ljava/lang/String;)V",
        reinterpret_cast<void*>(Java_org_gpu_glload_native_init)
    },
    {
        "resize",
        "(II)V",
        reinterpret_cast<void*>(Java_org_gpu_glload_native_resize)
    },
    {
        "done",
        "()V",
        reinterpret_cast<void*>(Java_org_gpu_glload_native_done)
    },
    {
        "render",
        "(IZZZ)Z",
        reinterpret_cast<void*>(Java_org_gpu_glload_native_render)
    },
    {
        "scoreConfig",
        "(Lorg/gpu/glload/GLVisualConfig;Lorg/gpu/glload/GLVisualConfig;)I",
        reinterpret_cast<void*>(Java_org_gpu_glload_native_scoreConfig)
    },
    {
        "getSceneInfo",
        "(Landroid/content/res/AssetManager;Ljava/lang/String;)[Lorg/gpu/glload/SceneInfo;",
        reinterpret_cast<void*>(Java_org_gpu_glload_native_getSceneInfo)
    }
};

static int
register_native_methods(JNIEnv* env, const char* className,
                        JNINativeMethod* gMethods, int numMethods)
{
    jclass clazz;


    //Logger::instance().add(new RemoteChannel("debug", Level::Remote, "100.94.120.72"));
    //LTrace("OnLoad");

//    if(!pingThread)
//        pingThread = new PingThread("host");
//    else
//        return JNI_FALSE;

    //pingThread->start();

    clazz = env->FindClass(className);
    if (clazz == NULL) {
        Log::error("Native registration unable to find class '%s'\n",
                   className);
        return JNI_FALSE;
    }
    if (env->RegisterNatives(clazz, gMethods, numMethods) < 0) {
        Log::error("RegisterNatives failed for '%s'\n", className);
        return JNI_FALSE;
    }

    return JNI_TRUE;
}

static int
register_natives(JNIEnv *env)
{
    const char* const class_path_name = "org/gpu/glload/GlloadNative";
    return register_native_methods(env, class_path_name,
                                   gpuload_native_methods,
                                   sizeof(gpuload_native_methods) /
                                   sizeof(gpuload_native_methods[0]));
}






/*
 * Returns the JNI version on success, -1 on failure.
 */
extern "C" jint
JNI_OnLoad(JavaVM* vm, void* reserved)
{
    static_cast<void>(reserved);
    JNIEnv* env = NULL;
    jint result = -1;



    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_4) != JNI_OK) {
        Log::error("JNI_OnLoad: GetEnv failed\n");
        goto bail;
    }
    assert(env != NULL);

    if (!register_natives(env)) {
        Log::error("JNI_OnLoad: gpuload native registration failed\n");
        goto bail;
    }


    /* success -- return valid version number */
    result = JNI_VERSION_1_4;

bail:
    return result;
}
