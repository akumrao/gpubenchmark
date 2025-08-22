
#ifndef GPULOAD_GL_STATE_H_
#define GPULOAD_GL_STATE_H_

#include <stdint.h>

class GLVisualConfig;

class GLState
{
public:
    virtual ~GLState() {}

    virtual bool init_display(void *native_display, GLVisualConfig& config_pref) = 0;
    virtual bool init_surface(void *native_window) = 0;
    virtual bool init_gl_extensions() = 0;
    virtual bool valid() = 0;
    virtual bool reset() = 0;
    virtual void swap() = 0;
    virtual bool gotNativeConfig(intptr_t& vid) = 0;
    virtual void getVisualConfig(GLVisualConfig& vc) = 0;
};

#endif /* GPULOAD_GL_STATE_H_ */
