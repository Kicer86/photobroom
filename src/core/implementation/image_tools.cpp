
#include "image_tools.hpp"

#include <any>

#include "iexif_reader.hpp"


namespace Image
{
    QImage normalized(const QString& src, IExifReader* exif)
    {
        QImage img(src);
        QImage rotated;

        if (img.isNull() == false)
        {
            const std::optional<std::any> orientation_raw = exif->get(src, IExifReader::TagType::Orientation);
            const int orientation = orientation_raw.has_value()?
                                        std::any_cast<int>(*orientation_raw):
                                        0;

            switch(orientation)
            {
                case 0:
                case 1:
                    rotated = img;
                    break;    // nothing to do - no data, or normal orientation

                case 2:
                    rotated = img.mirrored(true, false);
                    break;

                case 3:
                {
                    QTransform transform;
                    transform.rotate(180);

                    rotated = img.transformed(transform, Qt::SmoothTransformation);
                    break;
                }

                case 4:
                    rotated = img.mirrored(false, true);
                    break;

                case 5:
                {
                    QTransform transform;
                    transform.rotate(270);

                    rotated = img.mirrored(true, false).transformed(transform);
                    break;
                }

                case 6:
                {
                    QTransform transform;
                    transform.rotate(90);

                    rotated = img.transformed(transform, Qt::SmoothTransformation);
                    break;
                }

                case 7:
                {
                    QTransform transform;
                    transform.rotate(90);

                    rotated = img.mirrored(true, false).transformed(transform);
                    break;
                }

                case 8:
                {
                    QTransform transform;
                    transform.rotate(270);

                    rotated = img.transformed(transform);
                    break;
                }
            }
        }

        return rotated;
    }


    bool normalize(const QString& src, const QString& dst, IExifReader* exif)
    {
        const QImage n = normalized(src, exif);

        const bool success = n.isNull() == false;

        if (success)
            n.save(dst);

        return success;
    }
}
