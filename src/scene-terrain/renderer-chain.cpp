
#include "renderer.h"

void
RendererChain::setup_onscreen(Canvas &canvas)
{
    static_cast<void>(canvas);
}

void
RendererChain::setup_offscreen(const LibMatrix::vec2 &size, bool has_depth)
{
    static_cast<void>(size);
    static_cast<void>(has_depth);
}

void
RendererChain::setup_texture(GLint min_filter, GLint mag_filter,
                             GLint wrap_s, GLint wrap_t)
{
    static_cast<void>(min_filter);
    static_cast<void>(mag_filter);
    static_cast<void>(wrap_s);
    static_cast<void>(wrap_t);
}

void
RendererChain::input_texture(GLuint t)
{ 
    if (!renderers_.empty()) {
        IRenderer &renderer(*renderers_.front());
        renderer.input_texture(t);
    }
}

GLuint
RendererChain::texture()
{
    GLuint t(0);

    if (!renderers_.empty()) {
        IRenderer &renderer(*renderers_.back());
        t = renderer.texture();
    }

    return t;
}

LibMatrix::vec2
RendererChain::size()
{
    LibMatrix::vec2 s;

    if (!renderers_.empty()) {
        IRenderer &renderer(*renderers_.back());
        s = renderer.size();
    }

    return s;
}

void
RendererChain::make_current()
{
    if (!renderers_.empty()) {
        IRenderer &renderer(*renderers_.back());
        renderer.make_current();
    }
}

void
RendererChain::update_mipmap()
{
    if (!renderers_.empty()) {
        IRenderer &renderer(*renderers_.back());
        renderer.update_mipmap();
    }
}

void
RendererChain::render()
{
    for(std::vector<IRenderer *>::iterator iter = renderers_.begin();
        iter != renderers_.end();
        iter++)
    {
        (*iter)->render();
    }
}

void
RendererChain::append(IRenderer &renderer)
{
    if (!renderers_.empty()) {
        IRenderer &prev_renderer(*renderers_.back());
        renderer.input_texture(prev_renderer.texture());
    }

    renderers_.push_back(&renderer);
}
