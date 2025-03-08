//
// Created by stadmin on 20.07.2017.
//

#ifndef JAVACOPENGL_RENDERER_H
#define JAVACOPENGL_RENDERER_H
//is it because of only gl2?
#include <GLES2/gl2.h>
#ifdef ANDROID
#include <android/asset_manager.h>
#else
#include <dirent.h>
#endif

#include <string>
#include <sstream>

class Renderer {
public:
    // initializes shaders and gl buffers
    void init();

    //renders the openg buffers using the shader
    void draw();

    AAssetManager *android_asset_manager;


    std::istream * get_resource(const std::string &path);

    bool load_file(const std::string& filename, std::string& str);


private:
    GLuint mVertexBuffer;
    GLuint mIndexBuffer;

    GLuint mProgram;
    GLint mVertexAttribPos;

    unsigned int mElementCount = 0;
};


#endif //JAVACOPENGL_RENDERER_H
