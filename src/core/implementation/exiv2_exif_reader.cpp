
#include "exiv2_exif_reader.hpp"

#include <assert.h>

#include "base_tags.hpp"

namespace
{
    const std::map<AExifReader::TagTypes, const char *> exif_name =
    {
        { AExifReader::TagTypes::DateTimeOriginal, "Exif.Photo.DateTimeOriginal" },
        { AExifReader::TagTypes::Orientation,      "Exif.Image.Orientation" },
        { AExifReader::TagTypes::SequenceNumber,   "Exif.Sony1.SequenceNumber" }
    };
}


Exiv2ExifReader::Exiv2ExifReader (IPhotosManager* photosManager):
    AExifReader(photosManager),
    m_exif_data()
{

}


void Exiv2ExifReader::collect(const QByteArray& data)
{
    try
    {
        const unsigned char* udata = reinterpret_cast<const unsigned char *>(data.constData());
        m_exif_data = Exiv2::ImageFactory::open(udata, data.size());
    }
    catch (Exiv2::AnyError& error)
    {
        return;
    }

    assert(m_exif_data.get() != 0);
    m_exif_data->readMetadata();
}


std::string Exiv2ExifReader::read(AExifReader::TagTypes type) const
{
    std::string result;

    if (m_exif_data.get() != nullptr)
    {
        const Exiv2::ExifData &exifData = m_exif_data->exifData();

        if (exifData.empty() == false)
        {
            Exiv2::ExifData::const_iterator tag_data = exifData.end();
            auto exif_name_it = exif_name.find(type);

            if (exif_name_it != exif_name.end())
            {
                const char* tag_name = exif_name_it->second;
                tag_data = exifData.findKey(Exiv2::ExifKey(tag_name));
            }

            if (tag_data != exifData.end())
                result = tag_data->toString();
        }
    }

    return result;
}
