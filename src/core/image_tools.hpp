
#ifndef IMAGE_TOOLS_HPP
#define IMAGE_TOOLS_HPP

#include <QImage>

#include "core_export.h"

struct IExifReader;

namespace Image
{
    QImage CORE_EXPORT normalized(const QString &,
                                  IExifReader *);      // returns QImage rotated acordingly to exif data

    bool CORE_EXPORT normalize(const QString& src,
                               const QString& dst,
                               IExifReader *);         // save 'src' file as 'dst' with rotation data applied
}

#endif
