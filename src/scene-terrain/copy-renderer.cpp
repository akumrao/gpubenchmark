
#include "options.h"
#include "scene.h"
#include "renderer.h"
#include "shader-source.h"

CopyRenderer::CopyRenderer() :
    TextureRenderer(*copy_program(true))
{
    copy_program_ = copy_program(false);
}

Program *
CopyRenderer::copy_program(bool create_new)
{
    static Program *copy_program(0);
    if (create_new)
        copy_program = 0;

    if (!copy_program) {
        copy_program = new Program();
        ShaderSource vtx_shader(Options::data_path + "/shaders/terrain-texture.vert");
        ShaderSource frg_shader(Options::data_path + "/shaders/terrain-overlay.frag");

        Scene::load_shaders_from_strings(*copy_program, vtx_shader.str(), frg_shader.str());

        copy_program->start();
        (*copy_program)["tDiffuse"] = 0;
        (*copy_program)["opacity"] = 1.0f;
        (*copy_program)["uvOffset"] = LibMatrix::vec2(0.0f, 0.0f);
        (*copy_program)["uvScale"] = LibMatrix::vec2(1.0f, 1.0f);
        copy_program->stop();
    }

    return copy_program;
}
