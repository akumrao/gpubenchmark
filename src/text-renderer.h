
#ifndef GPULOAD_TEXT_RENDERER_H_
#define GPULOAD_TEXT_RENDERER_H_

#include <string>
#include "gl-headers.h"
#include "vec.h"
#include "program.h"
#include "canvas.h"

/**
 * Renders text using OpenGL textures.
 */
class TextRenderer
{
public:
    TextRenderer(Canvas& canvas);
    ~TextRenderer();

    void text(const std::string& t);
    void position(const LibMatrix::vec2& p);
    void size(float s);

    void render();

private:
    void create_geometry();
    LibMatrix::vec2 get_glyph_coords(char c);

    Canvas& canvas_;
    bool dirty_;
    std::string text_;
    LibMatrix::vec2 position_;
    LibMatrix::vec2 size_;
    Program program_;
    GLuint vbo_[2];
    GLuint texture_;
};

#endif
