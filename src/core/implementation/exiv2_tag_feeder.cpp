
#include "exiv2_tag_feeder.hpp"

#include <assert.h>

#include "base_tags.hpp"


Exiv2TagFeeder::Exiv2TagFeeder(IPhotosManager* photosManager):
    AExifReader(photosManager),
    m_exif_data()
{

}


void Exiv2TagFeeder::collect(const QByteArray& data)
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


std::string Exiv2TagFeeder::read(AExifReader::TagTypes type)
{
    std::string result;
    const Exiv2::ExifData &exifData = m_exif_data->exifData();

    if (exifData.empty() == false)
    {
        Exiv2::ExifData::const_iterator tag_date = exifData.end();

        switch (type)
        {
            case DateTimeOriginal:
                tag_date = exifData.findKey(Exiv2::ExifKey("Exif.Photo.DateTimeOriginal"));
                break;
        }

        if (tag_date != exifData.end())
            result = tag_date->toString();

        /*
         *   Exiv2::ExifData::const_iterator end = exifData.end();
         *   for (Exiv2::ExifData::const_iterator i = exifData.begin(); i != end; ++i)
         *   {
         *       const char* tn = i->typeName();
         *       std::cout << std::setw(44) << std::setfill(' ') << std::left
         *                   << i->key() << " "
         *                   << "0x" << std::setw(4) << std::setfill('0') << std::right
         *                   << std::hex << i->tag() << " "
         *                   << std::setw(9) << std::setfill(' ') << std::left
         *                   << (tn ? tn : "Unknown") << " "
         *                   << std::dec << std::setw(3)
         *                   << std::setfill(' ') << std::right
         *                   << i->count() << "  "
         *                   << std::dec << i->value()
         *                   << "\n";
         */
    }

    return result;
}
