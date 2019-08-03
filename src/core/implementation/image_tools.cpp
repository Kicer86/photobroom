
#include "image_tools.hpp"

#include <any>

#include "iexif_reader.hpp"


namespace Image
{
    OrientedImage normalized(const QString& src, IExifReader* exif)
    {
        return OrientedImage(exif, src);
    }


    bool normalize(const QString& src, const QString& dst, IExifReader* exif)
    {
        const OrientedImage oi = normalized(src, exif);
        const QImage n = oi.get();

        const bool success = n.isNull() == false;

        if (success)
            n.save(dst);

        return success;
    }
}
