
#ifndef IMAGE_TOOLS_HPP
#define IMAGE_TOOLS_HPP

#include <QImage>

struct IExifReader;

namespace Image
{
    QImage normalized(const QString &,
                      IExifReader *);      // returns QImage rotated acordingly to exif data

    void normalize(const QString& src,
                   const QString& dst,
                   IExifReader *);         // save 'src' file as 'dst' with rotation data applied
}

#endif
