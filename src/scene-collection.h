
#ifndef GPULOAD_SCENE_COLLECTION_H_
#define GPULOAD_SCENE_COLLECTION_H_

#include <vector>
#include "scene.h"


class SceneCollection
{
public:
    SceneCollection(Canvas& canvas) 
    {
        add_scenes(canvas);
    }
    ~SceneCollection() { Util::dispose_pointer_vector(scenes_); }
    void register_scenes()
    {
        for (std::vector<Scene*>::const_iterator iter = scenes_.begin();
             iter != scenes_.end();
             iter++)
        {
            Benchmark::register_scene(**iter);
        }
    }
    const std::vector<Scene*>& get() { return scenes_; }

private:
    std::vector<Scene*> scenes_;

    //
    // Creates all the available scenes and adds them to the supplied vector.
    // 
    // @param scenes the vector to add the scenes to
    // @param canvas the canvas to create the scenes with
    //
    void add_scenes(Canvas& canvas)
    {
        scenes_.push_back(new SceneDefaultOptions(canvas));
        scenes_.push_back(new SceneBuild(canvas));
        scenes_.push_back(new SceneTexture(canvas));
        scenes_.push_back(new SceneShading(canvas));
        scenes_.push_back(new SceneConditionals(canvas));
        scenes_.push_back(new SceneFunction(canvas));
        scenes_.push_back(new SceneLoop(canvas));
        scenes_.push_back(new SceneBump(canvas));
        scenes_.push_back(new SceneEffect2D(canvas));
        scenes_.push_back(new ScenePulsar(canvas));
        scenes_.push_back(new SceneDesktop(canvas));
        scenes_.push_back(new SceneBuffer(canvas));
        scenes_.push_back(new SceneIdeas(canvas));
        scenes_.push_back(new SceneTerrain(canvas));
        scenes_.push_back(new SceneJellyfish(canvas));
        scenes_.push_back(new SceneShadow(canvas));
        scenes_.push_back(new SceneRefract(canvas));
        scenes_.push_back(new SceneClear(canvas));

    }
};
#endif // GPULOAD_SCENE_COLLECTION_H_
