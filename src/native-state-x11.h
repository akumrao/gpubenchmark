
#ifndef GPULOAD_NATIVE_STATE_X11_H_
#define GPULOAD_NATIVE_STATE_X11_H_

#include "native-state.h"
#include <X11/Xlib.h>

class NativeStateX11 : public NativeState
{
public:
    NativeStateX11() : xdpy_(0), xwin_(0), properties_() {}
    ~NativeStateX11();

    bool init_display();
    void* display();
    bool create_window(WindowProperties const& properties);
    void* window(WindowProperties& properties);
    void visible(bool v);
    bool should_quit();
    void flip() { }

private:
    /** The X display associated with this canvas. */
    Display* xdpy_;
    /** The X window associated with this canvas. */
    Window xwin_;
    WindowProperties properties_;
};

#endif /* GPULOAD_NATIVE_STATE_X11_H_ */
