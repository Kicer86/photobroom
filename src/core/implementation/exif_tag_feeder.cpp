
#include "exif_tag_feeder.hpp"

#include <assert.h>

#include <QStringList>
#include <exiv2/exiv2.hpp>

#include <configuration/constants.hpp>

#include "base_tags.hpp"


ExifTagFeeder::ExifTagFeeder()
{

}


std::unique_ptr<ITagData> ExifTagFeeder::getTagsFor(const std::string &path)
{
    std::unique_ptr<ITagData> tagData(new TagData);
    feed(path, tagData.get());

    return tagData;
}


void ExifTagFeeder::update(ITagData *, const std::string &)
{

}


void ExifTagFeeder::feed(const std::string& path, ITagData* tagData)
{
    Exiv2::Image::AutoPtr image;

    try
    {
        image = Exiv2::ImageFactory::open(path);
    }
    catch
        (Exiv2::AnyError& error)
    {
        return;
    }

    assert(image.get() != 0);
    image->readMetadata();

    const Exiv2::ExifData &exifData = image->exifData();

    if (exifData.empty() == false)
    {

        Exiv2::ExifData::const_iterator tag_date = exifData.findKey(Exiv2::ExifKey("Exif.Photo.DateTimeOriginal"));

        if (tag_date != exifData.end())
        {
            QString v(tag_date->toString().c_str());
            QStringList time_splitted = v.split(" ");

            if (time_splitted.size() == 2)
            {
                QString date = time_splitted[0];
                const QString time = time_splitted[1];

                date.replace(":", ".");     //convert 2011:05:09 to 2011.05.09

                tagData->setTag(BaseTags::get(BaseTagsList::Date), date);
                tagData->setTag(BaseTags::get(BaseTagsList::Time), time);
            }
        }

        /*
        Exiv2::ExifData::const_iterator end = exifData.end();
        for (Exiv2::ExifData::const_iterator i = exifData.begin(); i != end; ++i)
        {
            const char* tn = i->typeName();
            std::cout << std::setw(44) << std::setfill(' ') << std::left
                        << i->key() << " "
                        << "0x" << std::setw(4) << std::setfill('0') << std::right
                        << std::hex << i->tag() << " "
                        << std::setw(9) << std::setfill(' ') << std::left
                        << (tn ? tn : "Unknown") << " "
                        << std::dec << std::setw(3)
                        << std::setfill(' ') << std::right
                        << i->count() << "  "
                        << std::dec << i->value()
                        << "\n";
        }
        */
    }
}
