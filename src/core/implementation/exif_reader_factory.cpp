
#include "exif_reader_factory.hpp"

#include <thread>

#include "exiv2_exif_reader.hpp"

#include "tag.hpp"
#include "iexif_reader.hpp"

namespace
{
    struct NullFeeder: public IExifReader
    {
            virtual ~NullFeeder() {}

        private:
            virtual Tag::TagsList getTagsFor(const QString&) override final
            {
                //return empty set
                return Tag::TagsList();
            }

    };
}


ExifReaderFactory::ExifReaderFactory(): m_feeders(), m_photosManager(nullptr)
{

}


void ExifReaderFactory::set(IPhotosManager* photosManager)
{
    m_photosManager = photosManager;
}


IExifReader* ExifReaderFactory::get()
{
    //ExifTool may not be thread safe. Prepare separate object for each thread
    const auto id = std::this_thread::get_id();
    auto it = m_feeders.find(id);

    if (it == m_feeders.end())
    {
        assert(m_photosManager != nullptr);

        auto feeder = std::make_unique<Exiv2ExifReader>(m_photosManager);
        auto in = m_feeders.emplace(id, std::move(feeder));

        it = in.first;
    }

    return it->second.get();
}
