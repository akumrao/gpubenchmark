
#include "options.h"
#include "scene.h"
#include "renderer.h"
#include "shader-source.h"

LibMatrix::vec2 SimplexNoiseRenderer::uv_scale_(1.5f, 1.5);

SimplexNoiseRenderer::SimplexNoiseRenderer() :
    TextureRenderer(*noise_program(true))
{
    noise_program_ = noise_program(false);
}

Program *
SimplexNoiseRenderer::noise_program(bool create_new)
{
    static Program *noise_program(0);
    if (create_new)
        noise_program = 0;

    if (!noise_program) {
        noise_program = new Program();
        ShaderSource vtx_shader(Options::data_path + "/shaders/terrain-texture.vert");
        ShaderSource frg_shader(Options::data_path + "/shaders/terrain-noise.frag");

        Scene::load_shaders_from_strings(*noise_program, vtx_shader.str(), frg_shader.str());

        noise_program->start();
        (*noise_program)["time"] = 1.0f;
        (*noise_program)["uvOffset"] = LibMatrix::vec2(0.0f, 0.0f);
        (*noise_program)["uvScale"] = uv_scale_;
        noise_program->stop();
    }

    return noise_program;
}
