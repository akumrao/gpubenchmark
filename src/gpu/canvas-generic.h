/*
 * This class is used when we run open gl from linux
 */


#ifndef GPULOAD_CANVAS_GENERIC_H_
#define GPULOAD_CANVAS_GENERIC_H_

#include "canvas.h"

class GLState;
class NativeState;

/**
 * Canvas for rendering with GL to an X11 window.
 */
class CanvasGeneric : public Canvas
{
public:
    CanvasGeneric(NativeState& native_state, GLState& gl_state,
                  int width, int height)
        : Canvas(width, height),
          native_state_(native_state), gl_state_(gl_state), native_window_(0),
          gl_color_format_(0), gl_depth_format_(0),
          color_renderbuffer_(0), depth_renderbuffer_(0), fbo_(0),
          window_initialized_(false) {}

    bool init();
    bool reset();
    void visible(bool visible);
    void clear();
    void update();
    void print_info();
    Pixel read_pixel(int x, int y);
    void write_to_file(std::string &filename);
    bool should_quit();
    void resize(int width, int height);
    unsigned int fbo();

private:
    bool supports_gl2();
    bool resize_no_viewport(int width, int height);
    bool do_make_current();
    bool ensure_gl_formats();
    bool ensure_fbo();
    void release_fbo();
    const char *get_gl_format_str(GLenum f);

    NativeState& native_state_;
    GLState& gl_state_;
    void* native_window_;
    GLenum gl_color_format_;
    GLenum gl_depth_format_;
    GLuint color_renderbuffer_;
    GLuint depth_renderbuffer_;
    GLuint fbo_;
    bool window_initialized_;
};

#endif /* GPULOAD_CANVAS_GENERIC_H_ */
