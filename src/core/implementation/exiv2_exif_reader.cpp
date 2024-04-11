
#include "exiv2_exif_reader.hpp"

#include <assert.h>

#include "base_tags.hpp"

namespace
{
    const std::map<AExifReader::TagType, std::string_view> exif_name =
    {
        { AExifReader::TagType::DateTimeOriginal, "Exif.Photo.DateTimeOriginal" },
        { AExifReader::TagType::Orientation,      "Exif.Image.Orientation" },
        { AExifReader::TagType::SequenceNumber,   "Exif.Sony1.SequenceNumber" },
        { AExifReader::TagType::PixelXDimension,  "Exif.Photo.PixelXDimension" },
        { AExifReader::TagType::PixelYDimension,  "Exif.Photo.PixelYDimension" },
        { AExifReader::TagType::Exposure,         "Exif.Photo.ExposureBiasValue" },
        { AExifReader::TagType::Projection,       "Xmp.GPano.ProjectionType" },
        { AExifReader::TagType::ShutterSpeed,     "Exif.Photo.ShutterSpeedValue" },
        { AExifReader::TagType::Xmp_video_DateTimeOriginal,     "Xmp.video.DateTimeOriginal" },
    };
}


Exiv2ExifReader::Exiv2ExifReader():
    m_exif_data(),
    m_path()
{

}


bool Exiv2ExifReader::hasExif(const QString& path)
{
    collect(path);

    return m_exif_data.get() && m_exif_data.get()->exifData().empty() == false;
}


void Exiv2ExifReader::collect(const QString& path)
{
    if (m_path != path)
    {
        try
        {
            m_exif_data.reset();

            m_exif_data = Exiv2::ImageFactory::open(path.toStdString());

            assert(m_exif_data.get() != 0);
            m_exif_data->readMetadata();
        }
        catch (const Exiv2::Error &)
        {
            return;
        }
    }

    // save path of file we are working on
    m_path = path;
}


std::optional<std::string> Exiv2ExifReader::read(AExifReader::TagType type) const
{
    std::optional<std::string> result;

    if (m_exif_data.get() != nullptr)
    {
        const Exiv2::ExifData& exifData = m_exif_data->exifData();
        const Exiv2::XmpData& xmpData = m_exif_data->xmpData();

        auto exif_name_it = exif_name.find(type);

        assert(exif_name_it != exif_name.end());

        if (exif_name_it != exif_name.end())
        {
            const auto tag_name = exif_name_it->second;
            if (tag_name.starts_with("Exif."))
            {
                auto tag_data = exifData.findKey(Exiv2::ExifKey(tag_name.data()));

                if (tag_data != exifData.end())
                    result = tag_data->toString();
            }
            else if (tag_name.starts_with("Xmp."))
            {
                auto tag_data = xmpData.findKey(Exiv2::XmpKey(tag_name.data()));

                if (tag_data != xmpData.end())
                    result = tag_data->toString();
            }
            else
                assert(!"Unknow tag family");
        }
    }

    return result;
}
