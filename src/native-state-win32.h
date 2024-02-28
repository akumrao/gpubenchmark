
#ifndef GPULOAD_NATIVE_STATE_WIN32_H_
#define GPULOAD_NATIVE_STATE_WIN32_H_

#include "native-state.h"

#include <windows.h>

class NativeStateWin32 : public NativeState
{
public:
    NativeStateWin32();
    ~NativeStateWin32();

    bool init_display();
    void* display();
    bool create_window(WindowProperties const& properties);
    void* window(WindowProperties& properties);
    void visible(bool v);
    bool should_quit();
    void flip();

private:
    void cleanup();
    void pump_messages();

    static LRESULT CALLBACK wnd_proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    HWND parent_window_;
    HWND child_window_;
    HDC native_display_;
    WindowProperties properties_;
    bool should_quit_;
};

#endif /* GPULOAD_NATIVE_STATE_H_ */
