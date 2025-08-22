
#ifndef GPULOAD_CANVAS_ANDROID_H_
#define GPULOAD_CANVAS_ANDROID_H_

#include "canvas.h"
#include "shared-library.h"
#include "glad/egl.h"

/**
 * Canvas for rendering to Android surfaces.
 *
 * This class doesn't perform any GLES2.0 surface and context management
 * (contrary to the CanvasX11* classes); these are handled in the Java
 * Android code.
 */
class CanvasAndroid : public Canvas
{
public:
    CanvasAndroid(int width, int height) :
        Canvas(width, height) {}
    ~CanvasAndroid() {}

    bool init();
    void visible(bool visible);
    void clear();
    void update();
    void print_info();
    Pixel read_pixel(int x, int y);
    void read_pixelbuf(char *pixels);
    void write_to_file(std::string &filename);
    bool should_quit();
    void resize(int width, int height);

private:
    SharedLibrary egl_lib_;
    SharedLibrary gles_lib_;
    GLVisualConfig chosen_config_;

    static GLADapiproc load_proc(void *userdata, const char *name);
    void init_gl_extensions();
};

#endif


