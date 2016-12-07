
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


std::string EasyExifTagFeeder::get(TagTypes type)
{
    std::string result;

    switch (type)
    {
        case DateTimeOriginal:
            result = m_exif_data.DateTimeOriginal;
            break;
    }

    return result;
}
