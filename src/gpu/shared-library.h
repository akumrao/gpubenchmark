
#ifndef GPULOAD_SHARED_LIBRARY_H_
#define GPULOAD_SHARED_LIBRARY_H_

#include <initializer_list>

class SharedLibrary
{
public:
    SharedLibrary();
    ~SharedLibrary();

    bool open(const char *name);
    bool open_from_alternatives(std::initializer_list<const char*> alt_names);
    void close();

    void *handle() const;
    void *load(const char *symbol) const;

private:
    void *handle_;
};

#endif /* GPULOAD_SHARED_LIBRARY_H_ */
