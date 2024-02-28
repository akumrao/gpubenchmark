
#include "renderer.h"

TextureRenderer::TextureRenderer(Program &program) :
    BaseRenderer(), program_(program)
{
    /* Create the mesh (quad) used for rendering */
    create_mesh();
}

void
TextureRenderer::create_mesh()
{
    // Set vertex format
    std::vector<int> vertex_format;
    vertex_format.push_back(3); // Position
    mesh_.set_vertex_format(vertex_format);

    mesh_.make_grid(1, 1, 2.0, 2.0, 0);
    mesh_.build_vbo();

    program_.start();

    // Set attribute locations
    std::vector<GLint> attrib_locations;
    attrib_locations.push_back(program_["position"].location());
    mesh_.set_attrib_locations(attrib_locations);

    program_.stop();
}

void
TextureRenderer::render()
{
    make_current();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, input_texture_);

    program_.start();

    mesh_.render_vbo();

    program_.stop();

    update_mipmap();
}
