
#include "options.h"
#include "scene.h"
#include "renderer.h"
#include "shader-source.h"

LuminanceRenderer::LuminanceRenderer() :
    TextureRenderer(*luminance_program(true))
{
    luminance_program_ = luminance_program(false);
}

Program *
LuminanceRenderer::luminance_program(bool create_new)
{
    static Program *luminance_program(0);
    if (create_new)
        luminance_program = 0;

    if (!luminance_program) {
        luminance_program = new Program();
        ShaderSource vtx_shader(Options::data_path + "/shaders/terrain-texture.vert");
        ShaderSource frg_shader(Options::data_path + "/shaders/terrain-luminance.frag");

        Scene::load_shaders_from_strings(*luminance_program,
                                         vtx_shader.str(), frg_shader.str());

        luminance_program->start();
        (*luminance_program)["tDiffuse"] = 0;
        (*luminance_program)["uvOffset"] = LibMatrix::vec2(0.0f, 0.0f);
        (*luminance_program)["uvScale"] = LibMatrix::vec2(1.0f, 1.0f);
        luminance_program->stop();
    }

    return luminance_program;
}


