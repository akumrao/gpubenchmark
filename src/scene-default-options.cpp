
#include "scene.h"
#include "benchmark.h"
#include "log.h"

bool
SceneDefaultOptions::setup()
{
    const std::map<std::string, Scene *> &scenes = Benchmark::scenes();

    for (std::list<std::pair<std::string, std::string> >::const_iterator iter = defaultOptions_.begin();
         iter != defaultOptions_.end();
         iter++)
    {
        for (std::map<std::string, Scene *>::const_iterator scene_iter = scenes.begin();
             scene_iter != scenes.end();
             scene_iter++)
        {
            Scene &scene(*(scene_iter->second));

            /* 
             * Display warning only if the option value is unsupported, not if
             * the scene doesn't support the option at all.
             */
            if (!scene.set_option_default(iter->first, iter->second) &&
                scene.options().find(iter->first) != scene.options().end())
            {
                Log::info("Warning: Scene '%s' doesn't accept default value '%s' for option '%s'\n",
                          scene.name().c_str(), iter->second.c_str(), iter->first.c_str());
            }
        }
    }

    return true;
}

bool
SceneDefaultOptions::set_option(const std::string &opt, const std::string &val)
{
    defaultOptions_.push_back(std::pair<std::string, std::string>(opt, val));
    return true;
}
