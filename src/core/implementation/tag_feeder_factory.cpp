
#include "tag_feeder_factory.hpp"

#include <thread>

#include "easy_exif_tag_feeder.hpp"

#include "tag.hpp"
#include "itagfeeder.hpp"

struct NullFeeder: public ITagFeeder
{
        virtual ~NullFeeder() {}

    private:
        virtual Tag::TagsList getTagsFor(const QString&) override final
        {
            //return empty set
            return Tag::TagsList();
        }

};


TagFeederFactory::TagFeederFactory(): m_feeders(), m_photosManager(nullptr)
{

}


void TagFeederFactory::set(IPhotosManager* photosManager)
{
    m_photosManager = photosManager;
}


std::shared_ptr<ITagFeeder> TagFeederFactory::get()
{
    //ExifTool may not be thread safe. Prepare separate object for each thread
    const auto id = std::this_thread::get_id();
    auto it = m_feeders.find(id);

    if (it == m_feeders.end())
    {
        auto feeder = std::make_shared<EasyExifTagFeeder>(m_photosManager);
        auto data = std::make_pair(id, feeder);
        auto in = m_feeders.insert(data);

        it = in.first;
    }

    return it->second;;
}
