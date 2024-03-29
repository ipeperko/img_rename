#include <boost/algorithm/string.hpp>
#include "FileItem.h"
#include <ImageMagick-7/Magick++.h>

FileItem::FileItem()
    : img(std::make_shared<Magick::Image>())
{
}

FileItem::FileItem(std::string file_name)
    : filename(std::move(file_name))
    , img(std::make_shared<Magick::Image>())
{
    img->read(filename);
    parseAttributes();
}

void FileItem::parseAttributes()
{
    std::vector<std::string> v = parseDateTime(*img);
    if (v.size() != 2) {
        throw std::domain_error("Unhandled date formats '" + img->attribute("EXIF:DateTimeOriginal") + " " + img->attribute("EXIF:DateTime") + "'");
    }

    capture_date = v[0];
    capture_time = v[1];
}

void FileItem::write(std::string file_name)
{
    filename = std::move(file_name);
    img->write(filename);
}

Magick::Image& FileItem::image()
{
    return *img.get();
}

Magick::Image const& FileItem::image() const
{
    return *img.get();
}

std::vector<std::string> FileItem::parseDateTime(Magick::Image& image)
{
    std::vector<std::string> v;

    auto split_ = [&](std::string_view s) {
        boost::split(v, s, boost::is_any_of(" "), boost::token_compress_on);
    };

    split_(image.attribute("EXIF:DateTimeOriginal"));

    if (v.empty()) {
        split_(image.attribute("EXIF:DateTime"));
    }

    return v;
}
