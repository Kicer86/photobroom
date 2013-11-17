
#include "tag_feeder.hpp"

#include <assert.h>

#include <exiv2/exiv2.hpp>

struct ExifFeeder
{
    void feed(const std::string& path, ITagData* tagData)
    {
        Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open(path);
        assert(image.get() != 0);
        image->readMetadata();

        Exiv2::ExifData &exifData = image->exifData();
        if (exifData.empty() == false)
        {
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
        }
    }
};


std::unique_ptr<ITagData> TagFeeder::getTagsFor(const std::string &path)
{

}


void TagFeeder::update(ITagData *tagData, const std::string &path)
{

}
