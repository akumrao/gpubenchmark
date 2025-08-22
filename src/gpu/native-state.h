/*
 *  This calls is called when we compile code with gcc on linux
 *
 */

#ifndef GPULOAD_NATIVE_STATE_H_
#define GPULOAD_NATIVE_STATE_H_

#include <stdint.h>

class NativeState
{
public:
    struct WindowProperties
    {
        WindowProperties(int w, int h, bool f, intptr_t v)
            : width(w), height(h), fullscreen(f), visual_id(v) {}
        WindowProperties()
            : width(0), height(0), fullscreen(false), visual_id(0) {}

        int width;
        int height;
        bool fullscreen;
        intptr_t visual_id;
    };

    virtual ~NativeState() {}

    /* Initializes the native display */
    virtual bool init_display() = 0;

    /* Gets the native display */
    virtual void* display() = 0;

    /* Creates (or recreates) the native window */
    virtual bool create_window(WindowProperties const& properties) = 0;

    /* 
     * Gets the native window and its properties.
     * The dimensions may be different than the ones requested.
     */
    virtual void* window(WindowProperties& properties) = 0;

    /* Sets the visibility of the native window */
    virtual void visible(bool v) = 0;

    /* Whether the user has requested an exit */
    virtual bool should_quit() = 0;

    /* Flips the display */
    virtual void flip() = 0;
};

#endif /* GPULOAD_NATIVE_STATE_H_ */
