/*
 * Jpeg and Png decoder and reader classes
 */



#include <png.h>
//#include <jpeglib.h>
#include <cstring>
#include <memory>

#include "image-reader.h"
#include "log.h"
#include "util.h"

/*******
 * PNG *
 *******/

struct PNGReaderPrivate
{
    PNGReaderPrivate() :
        png(0), info(0), rows(0), png_error(0),
        current_row(0), row_stride(0) {}

    static void png_read_fn(png_structp png_ptr, png_bytep data, png_size_t length)
    {
        std::istream *is = reinterpret_cast<std::istream*>(png_get_io_ptr(png_ptr));
        is->read(reinterpret_cast<char *>(data), length);
    }

    png_structp png;
    png_infop info;
    png_bytepp rows;
    bool png_error;
    unsigned int current_row;
    unsigned int row_stride;
};

PNGReader::PNGReader(const std::string& filename):
    priv_(new PNGReaderPrivate())
{
    priv_->png_error = !init(filename);
}

PNGReader::~PNGReader()
{
    finish();
    delete priv_;
}

bool
PNGReader::error()
{
    return priv_->png_error;
}

bool
PNGReader::nextRow(unsigned char *dst)
{
    bool ret;

    if (priv_->current_row < height()) {
        memcpy(dst, priv_->rows[priv_->current_row], priv_->row_stride);
        priv_->current_row++;
        ret = true;
    }
    else {
        ret = false;
    }

    return ret;
}

unsigned int
PNGReader::width() const
{ 
    return png_get_image_width(priv_->png, priv_->info);
}

unsigned int
PNGReader::height() const
{ 
    return png_get_image_height(priv_->png, priv_->info);
}

unsigned int
PNGReader::pixelBytes() const
{
    if (png_get_color_type(priv_->png, priv_->info) == PNG_COLOR_TYPE_RGB)
    {
        return 3;
    }
    return 4;
}


bool
PNGReader::init(const std::string& filename)
{
    static const int png_transforms = PNG_TRANSFORM_STRIP_16 |
                                      PNG_TRANSFORM_GRAY_TO_RGB |
                                      PNG_TRANSFORM_PACKING |
                                      PNG_TRANSFORM_EXPAND;

    Log::debug("Reading PNG file %s\n", filename.c_str());

    const std::unique_ptr<std::istream> is_ptr(Util::get_resource(filename));
    if (!(*is_ptr)) {
        Log::error("Cannot open file %s!\n", filename.c_str());
        return false;
    }

    /* Set up all the libpng structs we need */
    priv_->png = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
    if (!priv_->png) {
        Log::error("Couldn't create libpng read struct\n");
        return false;
    }

    priv_->info = png_create_info_struct(priv_->png);
    if (!priv_->info) {
        Log::error("Couldn't create libpng info struct\n");
        return false;
    }

    /* Set up libpng error handling */
    if (setjmp(png_jmpbuf(priv_->png))) {
        Log::error("libpng error while reading file %s\n", filename.c_str());
        return false;
    }

    /* Read the image information and data */
    png_set_read_fn(priv_->png, reinterpret_cast<void*>(is_ptr.get()),
                    PNGReaderPrivate::png_read_fn);

    png_read_png(priv_->png, priv_->info, png_transforms, 0);

    priv_->rows = png_get_rows(priv_->png, priv_->info);

    priv_->current_row = 0;
    priv_->row_stride = width() * pixelBytes();

    return true;
}

void
PNGReader::finish()
{
    if (priv_->png)
    {
        png_destroy_read_struct(&priv_->png, &priv_->info, 0);
    }
}




