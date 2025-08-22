
#ifndef GPULOAD_GL_STATE_WGL_H_
#define GPULOAD_GL_STATE_WGL_H_

#include "gl-state.h"
#include "shared-library.h"
#include <windows.h>

class GLStateWGL : public GLState
{
public:
    GLStateWGL();
    ~GLStateWGL();

    bool init_display(void* native_display, GLVisualConfig& config_pref);
    bool init_surface(void* native_window);
    bool init_gl_extensions();
    bool valid();
    bool reset();
    void swap();
    bool gotNativeConfig(intptr_t& vid);
    void getVisualConfig(GLVisualConfig& vc);

private:
    using api_proc = void (*)();
    static api_proc load_proc(void *userptr, const char *name);

    SharedLibrary wgl_library_;
    HDC hdc_;
    HGLRC wgl_context_;
    void* get_proc_addr_;
};

#endif // GPULOAD_GL_STATE_WGL_H_
