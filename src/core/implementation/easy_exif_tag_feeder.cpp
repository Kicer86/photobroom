
#include "easy_exif_tag_feeder.hpp"

#include <assert.h>

#include <QByteArray>


EasyExifTagFeeder::EasyExifTagFeeder(IPhotosManager* photosManager): m_exif_data()
{
    AExifReader::set(photosManager);
}


void EasyExifTagFeeder::collect(const QByteArray& data)
{
    const unsigned char* rawData = reinterpret_cast<const unsigned char *>(data.data());
    const int rawDataSize = data.size();

    m_exif_data.parseFrom(rawData, static_cast<unsigned int>(rawDataSize));
}


std::string EasyExifTagFeeder::read(Tag::Types type)
{
    std::string result;

    switch (type)
    {
        case DateTimeOriginal:
            result = m_exif_data.DateTimeOriginal;
            break;

        default:
            assert(!"missing implementation");
            break;
    }

    return result;
}
