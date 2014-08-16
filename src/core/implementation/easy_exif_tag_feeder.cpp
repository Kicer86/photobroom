
#include "easy_exif_tag_feeder.hpp"

#include <assert.h>

#include <QByteArray>


EasyExifTagFeeder::EasyExifTagFeeder()
{

}


void EasyExifTagFeeder::collect(const QByteArray& data)
{
    const unsigned char* rawData = reinterpret_cast<const unsigned char *>(data.data());
    const std::size_t rawDataSize = data.size();

    m_exif_data.parseFrom(rawData, rawDataSize);
}


std::string EasyExifTagFeeder::get(TagTypes type)
{
    std::string result;

    switch (type)
    {
        case DateTimeOriginal:
            result = m_exif_data.DateTimeOriginal;
    }

    return result;
}
