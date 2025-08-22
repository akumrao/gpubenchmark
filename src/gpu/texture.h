
#ifndef GPULOAD_TEXTURE_H_
#define GPULOAD_TEXTURE_H_

#include "gl-headers.h"

#include <string>
#include <map>

/**
 * A descriptor for a texture file.
 */
class TextureDescriptor
{
public:
    enum FileType {
        FileTypeUnknown,
        FileTypePNG,
        FileTypeJPEG,
    };

    TextureDescriptor(const std::string& name, const std::string& pathname,
                      FileType filetype) :
        name_(name),
        pathname_(pathname),
        filetype_(filetype) {}
    ~TextureDescriptor() {}
    const std::string& pathname() const { return pathname_; }
    FileType filetype() const { return filetype_; }
private:
    std::string name_;
    std::string pathname_;
    FileType filetype_;
    TextureDescriptor();
};

typedef std::map<std::string, TextureDescriptor*> TextureMap;

class Texture
{
public:
    /**
     * Load a texture by name.
     *
     * You must initialize the available texture collection using
     * Texture::find_textures() before using this method.
     *
     * @name:        the texture name
     *
     * @return:      true if the operation succeeded, false otherwise
     */
    static bool load(const std::string &name, GLuint *pTexture, ...);
    /**
     * Locate all available textures.
     *
     * This method scans the built-in data paths and builds a database of usable
     * textures available to scenes.  Map is available on a read-only basis to
     * scenes that might find it useful for listing textures, etc.
     *
     * @return:     a map containing information about the located textures
     */
    static const TextureMap& find_textures();
};

#endif
