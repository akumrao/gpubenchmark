/*
 *  Base calls for all the the scenes, all the logic for tiles, wallpaper, golden frame buffer comparison are done here
 *
 */

#ifndef GPULOAD_SCENE_H_
#define GPULOAD_SCENE_H_

#include "gl-headers.h"

#include "mesh.h"
#include "vec.h"
#include "program.h"

#include <math.h>

#include <string>
#include <map>
#include <list>
#include <vector>
#include "canvas.h"
#include "json/json.hpp"
#include "json/configuration.h"
#include "default-benchmarks.h"

using json = nlohmann::json;


struct stRgbInf{
    std::string file;
    int w;
    int h;
};

/**
 * A configurable scene used for creating benchmarks.
 */
class Scene
{
public:
    virtual ~Scene();

    long int ms;

    std::string storagePath;

//    struct Stats {
//        json root;
//    };


//    static struct Stats stats;
    void Settats(json &cnfg);

   // base::cnfg::Configuration statFile;

    /**
     * Scene options.
     */
    struct Option {
        Option(const std::string &nam, const std::string &val, const std::string &desc,
               const std::string &values = "");

        Option() {}
        std::string name;
        std::string value;
        std::string default_value;
        std::string description;
        std::vector<std::string> acceptable_values;
        bool set;
    };

    /**
     * The result of a validation check.
     */
    enum ValidationResult {
        ValidationFailure,
        ValidationSuccess,
        ValidationUnknown
    };

    /**
     * Checks whether this scene (in its current configuration) is supported.
     *
     * @param show_errors whether to log errors about unsupported features
     *
     * @return whether the scene is supported
     */
    virtual bool supported(bool show_errors);

    /**
     * Performs option-independent resource loading and configuration.
     *
     * It should be safe to call ::load() (and the corresponding ::unload())
     * only once per program execution, although you may choose to do so more
     * times to better manage resource consumption.
     *
     * @return whether loading succeeded
     */
    virtual bool load();

    /**
     * Performs option-independent resource unloading.
     */
    virtual void unload();

    /**
     * Performs option-dependent resource loading and configuration.
     *
     * This method also prepares a scene for a benchmark run.
     * It should be called just before running a scene/benchmark.
     *
     * The base Scene::setup() method also checks whether a scene
     * configuration is supported by calling ::supported(true).
     *
     * @return whether setting the scene up succeeded
     */
    virtual bool setup();

    /**
     * Performs option-dependent resource unloading.
     *
     * This method should be called just after running a scene/benchmark.
     *
     * @return the operation status
     */
    virtual void teardown();

    /**
     * Updates the scene state.
     */
    virtual void update();

    /**
     * Draws the current scene state.
     */
    virtual void draw();

    virtual void statsInit(Config & config);
    virtual void statsRun(bool showFullStats, Config & config);
    virtual void statsStop();

    void gpuinit(bool showFullStats, Config& config);

    void gpustats(bool showFullStats, Config& config);

    int cur_freq;
    int utilization;
    int temp;
    int min_freq;
    int max_freq;
    double cur_freq_per;
    int dev_cur_freq;
    int battery;
    bool isPowSaveMode;
    float gpuEng{0};
    float gpuCam{0};
    float gpuDDR{0};
    float gpuDisplay{0};

   // bool isSustainedPerformanceModeSupported;
    std::map<std::string, std::string> rail_mapping;



    /**
     * Gets an informational string describing the scene.
     *
     * @param title if specified, a custom title to use, instead of the default
     */
    virtual std::string info_string(const std::string &title = "");

    /**
     * Sets the value of an option for this scene.
     *
     * @param opt the option to set
     * @param val the value to set the option to
     *
     * @return whether the option value was set successfully
     */
    virtual bool set_option(const std::string &opt, const std::string &val);

    /**
     * Validates the current output of this scene.
     *
     * This method should be called after having called ::draw() once.
     *
     * @return the validation result
     */
    virtual ValidationResult validate(std::string &file, int &w , int &h) ;//{ return ValidationUnknown; }

     bool shouldvalidate() ;


    /**
     * Gets whether this scene is running.
     *
     * @return true if running, false otherwise
     */
    bool running() { return running_; }

    /**
     * Sets whether this scene is running.
     *
     * @return true if running, false otherwise
     */
    void running(bool r) { running_ = r; }

    /**
     * Gets the average FPS value for this scene.
     *
     * @return the average FPS value
     */
    unsigned average_fps();

    /**
     * Gets the name of the scene.
     * @return the name of the scene
     */
    const std::string &name() { return name_; }

    /**
     * Resets all scene options to their default values.
     */
    void reset_options();

    /**
     * Sets the default value of a scene option.
     */
    bool set_option_default(const std::string &opt, const std::string &val);

    /**
     * Gets the scene options.
     *
     * @return the scene options
     */
    const std::map<std::string, Option> &options() { return options_; }

    /**
     * Gets a dummy scene object reference.
     *
     * @return the dummy Scene
     */
    static Scene &dummy()
    {
        static Scene dummy_scene(Canvas::dummy(), "");
        return dummy_scene;
    }

    /**
     * Loads a shader program from a pair of vertex and fragment shader strings.
     *
     * @return whether the operation succeeded
     */
    static bool load_shaders_from_strings(Program &program,
                                          const std::string &vtx_shader,
                                          const std::string &frg_shader,
                                          const std::string &vtx_shader_filename = "None",
                                          const std::string &frg_shader_filename = "None");


    bool match();
    std::vector<stRgbInf> clipstorage;
protected:
    Scene(Canvas &pCanvas, const std::string &name);
    std::string construct_title(const std::string &title);

    Canvas &canvas_;
    std::string name_;
    std::map<std::string, Option> options_;
    double startTime_;
    double lastUpdateTime_;
    unsigned currentFrame_;
    bool running_;
    double duration_;      // Duration of run in seconds
    unsigned nframes_;
    int frameCount{0};
    Scene::ValidationResult validationPassed{Scene::ValidationSuccess};
};

/*
 * Special Scene used for setting the default options
 */
class SceneDefaultOptions : public Scene
{
public:
    SceneDefaultOptions(Canvas &pCanvas) : Scene(pCanvas, "") {}
    bool set_option(const std::string &opt, const std::string &val);
    bool setup();

private:
    std::list<std::pair<std::string, std::string> > defaultOptions_;
};

class SceneBuild : public Scene
{
public:
    SceneBuild(Canvas &pCanvas);
    bool load();
    void unload();
    bool setup();
    void teardown();
    void update();
    void draw();
   // ValidationResult validate();

    ~SceneBuild();

protected:
    Program program_;
    LibMatrix::mat4 perspective_;
    LibMatrix::vec3 centerVec_;
    float radius_;
    Mesh mesh_;
    bool orientModel_;
    float orientationAngle_;
    LibMatrix::vec3 orientationVec_;
    float rotation_;
    float rotationSpeed_;
    bool useVbo_;
};

class SceneTexture : public Scene
{
public:
    SceneTexture(Canvas &pCanvas);
    bool load();
    void unload();
    bool setup();
    void teardown();
    void update();
    void draw();

    ~SceneTexture();

protected:
    Program program_;
    Mesh mesh_;
    GLuint texture_;
    float radius_;
    bool orientModel_;
    float orientationAngle_;
    LibMatrix::vec3 orientationVec_;
    LibMatrix::mat4 perspective_;
    LibMatrix::vec3 centerVec_;
    LibMatrix::vec3 rotation_;
    LibMatrix::vec3 rotationSpeed_;
};

class SceneShading : public Scene
{
public:
    SceneShading(Canvas &pCanvas);
    bool load();
    void unload();
    bool setup();
    void teardown();
    void update();
    void draw();

    ~SceneShading();

protected:
    Program program_;
    float radius_;
    bool orientModel_;
    float orientationAngle_;
    LibMatrix::vec3 orientationVec_;
    LibMatrix::vec3 centerVec_;
    LibMatrix::mat4 perspective_;
    Mesh mesh_;
    float rotation_;
    float rotationSpeed_;
};

class SceneGrid : public Scene
{
public:
    SceneGrid(Canvas &pCanvas, const std::string &name);
    virtual bool load();
    virtual void unload();
    virtual bool setup();
    virtual void teardown();
    virtual void update();
    virtual void draw();

    ~SceneGrid();

protected:
    Program program_;
    Mesh mesh_;
    float rotation_;
    float rotationSpeed_;
};

class SceneConditionals : public SceneGrid
{
public:
    SceneConditionals(Canvas &pCanvas);
    bool setup();

    ~SceneConditionals();
};

class SceneFunction : public SceneGrid
{
public:
    SceneFunction(Canvas &pCanvas);
    bool setup();
    ~SceneFunction();
};

class SceneLoop : public SceneGrid
{
public:
    SceneLoop(Canvas &pCanvas);
    bool setup();

    ~SceneLoop();
};

class SceneBump : public Scene
{
public:
    SceneBump(Canvas &pCanvas);
    bool load();
    void unload();
    bool setup();
    void teardown();
    void update();
    void draw();
    ~SceneBump();

protected:
    Program program_;
    Mesh mesh_;
    GLuint texture_;
    float rotation_;
    float rotationSpeed_;
private:
    bool setup_model_plain(const std::string &type);
    bool setup_model_normals();
    bool setup_model_normals_tangent();
    bool setup_model_height();
};

class SceneEffect2D : public Scene
{
public:
    SceneEffect2D(Canvas &pCanvas);
    bool load();
    void unload();
    bool setup();
    void teardown();
    void update();
    void draw();

    ~SceneEffect2D();

protected:
    Program program_;

    Mesh mesh_;
    GLuint texture_;
};

class ScenePulsar : public Scene
{
public:
    ScenePulsar(Canvas &pCanvas);
    bool load();
    void unload();
    bool setup();
    void teardown();
    void update();
    void draw();

    ~ScenePulsar();

protected:
    int numQuads_;
    Program program_;
    Mesh mesh_;
    LibMatrix::vec3 scale_;
    std::vector<LibMatrix::vec3> rotations_;
    std::vector<LibMatrix::vec3> rotationSpeeds_;
    GLuint texture_;

private:
    void create_and_setup_mesh();
};

struct SceneDesktopPrivate;

class SceneDesktop : public Scene
{
public:
    SceneDesktop(Canvas &canvas);
    bool supported(bool show_errors);
    bool load();
    void unload();
    bool setup();
    void teardown();
    void update();
    void draw();

    ~SceneDesktop();

private:
    SceneDesktopPrivate *priv_;
};



class SceneBuffer : public Scene
{
public:
    SceneBuffer(Canvas &pCanvas);
    bool load();
    bool runit();
    void unload();
    bool setup();
    void teardown();
    void update();
    void draw();

    void loadtx(std::string name, GLuint *tex,  GLint min_filter, GLint mag_filter);
    void loadmultitx(std::string name, GLuint *tex,  GLint min_filter, GLint mag_filter);

    ~SceneBuffer();





    std::vector<stRgbInf>::iterator itStore;  // declare an iterator to a vector of strings

    int nCount =1;
    unsigned char *m_rgb{nullptr};
    void readfile(std::string file, int w, int h,  int pos);
    bool done{false};

protected:
    Program program_;

    Mesh mesh_;
    GLuint texture_;
};


class SceneIdeasPrivate;

class SceneIdeas : public Scene
{
public:
    SceneIdeas(Canvas &pCanvas);
    bool load();
    void unload();
    bool setup();
    void teardown();
    void update();
    void draw();

    ~SceneIdeas();

private:
    SceneIdeasPrivate* priv_;
};

class SceneTerrainPrivate;

//#define FC  1
class SceneTerrain : public Scene
{
public:
    SceneTerrain(Canvas &pCanvas);
    bool supported(bool show_errors);
    bool load();
    void unload();
    bool setup();
    void teardown();
    void update();
    void draw();

    ~SceneTerrain();

private:
    SceneTerrainPrivate* priv_;
};

class JellyfishPrivate;
class SceneJellyfish : public Scene
{
    JellyfishPrivate* priv_;
public:
    SceneJellyfish(Canvas &pCanvas);
    ~SceneJellyfish();
    bool load();
    void unload();
    bool setup();
    void teardown();
    void update();
    void draw();
 
};

class ShadowPrivate;
class SceneShadow : public Scene
{
    ShadowPrivate* priv_;
public:
    SceneShadow(Canvas& canvas);
    bool supported(bool show_errors);
    bool load();
    void unload();
    bool setup();
    void teardown();
    void update();
    void draw();
};

class RefractPrivate;
class SceneRefract : public Scene
{
    RefractPrivate* priv_;
public:
    SceneRefract(Canvas& canvas);
    bool supported(bool show_errors);
    bool load();
    void unload();
    bool setup();
    void teardown();
    void update();
    void draw();
};

class SceneClear : public Scene
{
public:
    SceneClear(Canvas &pCanvas);
    bool load();
    void unload();
    bool setup();
    void teardown();
    void update();
    void draw();
    //ValidationResult validate();
};

#endif
