/*
 *  This calls is called when we compile code with gcc on linux
 *
 */

#ifndef GPULOAD_NATIVE_STATE_MIR_H_
#define GPULOAD_NATIVE_STATE_MIR_H_

#include "native-state.h"
#include <mir_toolkit/mir_client_library.h>
#include <csignal>

class NativeStateMir : public NativeState
{
public:
    NativeStateMir() : mir_connection_(0), mir_surface_(0), properties_() {}
    ~NativeStateMir();

    bool init_display();
    void* display();
    bool create_window(WindowProperties const& properties);
    void* window(WindowProperties& properties);
    void visible(bool v);
    bool should_quit();
    void flip() { }

private:
    static void quit_handler(int signum);
    static volatile std::sig_atomic_t should_quit_;

    MirConnection* mir_connection_;
    MirSurface* mir_surface_;
    WindowProperties properties_;
};

#endif /* GPULOAD_NATIVE_STATE_MIR_H_ */
