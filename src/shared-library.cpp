
#include "shared-library.h"

#include <stdio.h>

#if defined(WIN32)
#include <windows.h>
#else
#include <dlfcn.h>
#endif

/******************
 * Public methods *
 ******************/
SharedLibrary::SharedLibrary() : handle_(nullptr) {}

SharedLibrary::~SharedLibrary()
{
    close();
}

bool SharedLibrary::open(const char *libName)
{
#if defined(WIN32)
    handle_ = LoadLibraryA(libName);
#else
    handle_ = dlopen(libName, RTLD_NOW | RTLD_NODELETE);
#endif
    return (handle_ != nullptr);
}

bool SharedLibrary::open_from_alternatives(std::initializer_list<const char*> alt_names)
{
    for (const char *name : alt_names) {
        if (open(name))
            return true;
    }

    return false;
}

void SharedLibrary::close()
{
    if (handle_)
    {
#if defined(WIN32)
        FreeLibrary(reinterpret_cast<HMODULE>(handle_));
#else
        dlclose(handle_);
#endif
    }
}

void *SharedLibrary::handle() const
{
    return handle_;
}

void *SharedLibrary::load(const char *symbol) const
{
#if defined(WIN32)
    return reinterpret_cast<void*>(GetProcAddress(reinterpret_cast<HMODULE>(handle_), symbol));
#else
    return dlsym(handle_, symbol);
#endif
}

/*******************
 * Private methods *
 *******************/
