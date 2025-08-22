
#include "scene.h"
#include "util.h"

SceneClear::SceneClear(Canvas& canvas) :
    Scene(canvas, "clear")
{
}

bool
SceneClear::load()
{
    running_ = false;
    return true;
}

void
SceneClear::unload()
{
}

bool
SceneClear::setup()
{
    if (!Scene::setup())
        return false;

    // Add scene-specific setup code here

    // Set up the frame timing values and
    // indicate that the scene is ready to run.
    running_ = true;
    startTime_ = Util::get_timestamp_us() / 1000000.0;
    lastUpdateTime_ = startTime_;
    return true;
}

void
SceneClear::teardown()
{
    // Add scene-specific teardown code here
    Scene::teardown();
}

void
SceneClear::update()
{
    Scene::update();
    // Add scene-specific update code here
}

void
SceneClear::draw()
{
}

//Scene::ValidationResult
//SceneClear::validate()
//{
//    return Scene::ValidationUnknown;
//}
