
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

/**
 * A configurable scene used for creating benchmarks.
 */
class Scene
{
public:
    virtual ~Scene();

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

    virtual void statsInit();
    virtual void statsRun();
    virtual void statsStop();

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
    virtual ValidationResult validate() { return ValidationUnknown; }

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
    ValidationResult validate();

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
    ValidationResult validate();

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
    ValidationResult validate();

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
    virtual ValidationResult validate();

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
    ValidationResult validate();

    ~SceneConditionals();
};

class SceneFunction : public SceneGrid
{
public:
    SceneFunction(Canvas &pCanvas);
    bool setup();
    ValidationResult validate();

    ~SceneFunction();
};

class SceneLoop : public SceneGrid
{
public:
    SceneLoop(Canvas &pCanvas);
    bool setup();
    ValidationResult validate();

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
    ValidationResult validate();

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
    ValidationResult validate();

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
    ValidationResult validate();

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
    ValidationResult validate();

    ~SceneDesktop();

private:
    SceneDesktopPrivate *priv_;
};

struct SceneBufferPrivate;

class SceneBuffer : public Scene
{
public:
    SceneBuffer(Canvas &canvas);
    bool supported(bool show_errors);
    bool load();
    void unload();
    bool setup();
    void teardown();
    void update();
    void draw();
    ValidationResult validate();

    ~SceneBuffer();

private:
    SceneBufferPrivate *priv_;
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
    ValidationResult validate();

    ~SceneIdeas();

private:
    SceneIdeasPrivate* priv_;
};

class SceneTerrainPrivate;

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
    ValidationResult validate();

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
    ValidationResult validate();
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
    ValidationResult validate();
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
    ValidationResult validate();
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
    ValidationResult validate();
};

#endif
