
#include <string>

class ImageReader
{
public:
    virtual bool error() = 0;
    virtual bool nextRow(unsigned char *dst) = 0;
    virtual unsigned int width() const = 0;
    virtual unsigned int height() const = 0;
    virtual unsigned int pixelBytes() const = 0;
    virtual ~ImageReader() {}
};

struct PNGReaderPrivate;

class PNGReader : public ImageReader
{
public:
    PNGReader(const std::string& filename);

    virtual ~PNGReader();
    bool error();
    bool nextRow(unsigned char *dst);

    unsigned int width() const;
    unsigned int height() const;
    unsigned int pixelBytes() const;

private:
    bool init(const std::string& filename);
    void finish();

    PNGReaderPrivate *priv_;
};

struct JPEGReaderPrivate;

class JPEGReader : public ImageReader
{
public:
    JPEGReader(const std::string& filename);

    virtual ~JPEGReader();
    bool error();
    bool nextRow(unsigned char *dst);
    unsigned int width() const;
    unsigned int height() const;
    unsigned int pixelBytes() const;

private:
    bool init(const std::string& filename);
    void finish();

    JPEGReaderPrivate *priv_;
};

