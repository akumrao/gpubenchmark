/*
 *  This calls is called when we compile code with gcc on linux
 *
 */


#ifndef GPULOAD_NATIVE_STATE_DISPMANX_H_
#define GPULOAD_NATIVE_STATE_DISPMANX_H_

#include <vector>
#include "bcm_host.h"
#include "GLES/gl.h"
#include "EGL/egl.h"
#include "EGL/eglext.h"

#include "native-state.h"

class NativeStateDispmanx : public NativeState
{
public:
    NativeStateDispmanx();
    ~NativeStateDispmanx();

    bool init_display();
    void* display();
    bool create_window(WindowProperties const& properties);
    void* window(WindowProperties& properties);
    void visible(bool v);
    bool should_quit();
    void flip();

private:
    DISPMANX_DISPLAY_HANDLE_T dispmanx_display;
    DISPMANX_UPDATE_HANDLE_T dispmanx_update;
    DISPMANX_ELEMENT_HANDLE_T dispmanx_element;
    EGL_DISPMANX_WINDOW_T egl_dispmanx_window;
    WindowProperties properties_;
};

#endif /* GPULOAD_NATIVE_STATE_DISPMANX_H_ */
