
#include "exiv2_exif_reader.hpp"

#include <assert.h>

#include "base_tags.hpp"


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
    const Exiv2::ExifData &exifData = m_exif_data->exifData();

    if (exifData.empty() == false)
    {
        Exiv2::ExifData::const_iterator tag_date = exifData.end();

        switch (type)
        {
            case SequenceNumber:
                tag_date = exifData.findKey(Exiv2::ExifKey("Exif.Sony1.SequenceNumber"));
                break;

            case DateTimeOriginal:
                tag_date = exifData.findKey(Exiv2::ExifKey("Exif.Photo.DateTimeOriginal"));
                break;
        }

        if (tag_date != exifData.end())
            result = tag_date->toString();
    }

    return result;
}
