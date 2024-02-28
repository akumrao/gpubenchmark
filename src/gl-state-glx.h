
#ifndef GPULOAD_GL_STATE_GLX_H_
#define GPULOAD_GL_STATE_GLX_H_

#include "gl-state.h"
#include "gl-visual-config.h"
#include "gl-headers.h"
#include "shared-library.h"

#include <vector>

#include <glad/glx.h>

class GLStateGLX : public GLState
{
public:
    GLStateGLX()
        : xdpy_(0), xwin_(0), glx_fbconfig_(0), glx_context_(0) {}

    bool valid();
    bool init_display(void* native_display, GLVisualConfig& config_pref);
    bool init_surface(void* native_window);
    bool init_gl_extensions();
    bool reset();
    void swap();
    bool gotNativeConfig(intptr_t& vid);
    void getVisualConfig(GLVisualConfig& vc);

private:
    bool check_glx_version();
    void init_extensions();
    bool ensure_glx_fbconfig();
    bool ensure_glx_context();
    void get_glvisualconfig_glx(GLXFBConfig config, GLVisualConfig &visual_config);
    GLXFBConfig select_best_config(std::vector<GLXFBConfig> configs);

    static GLADapiproc load_proc(void* userptr, const char* name);

    Display* xdpy_;
    Window xwin_;
    GLXFBConfig glx_fbconfig_;
    GLXContext glx_context_;
    GLVisualConfig requested_visual_config_;
    SharedLibrary lib_;
};

#endif /* GPULOAD_GL_STATE_GLX_H_ */
