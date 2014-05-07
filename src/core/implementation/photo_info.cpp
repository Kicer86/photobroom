
#include "photo_info.hpp"

#include <memory>
#include <mutex>

#include <QPixmap>

#include <OpenLibrary/palgorithm/ts_resource.hpp>

#include "tag.hpp"
#include "itagfeeder.hpp"
#include "task_executor.hpp"


struct PhotoInfo::Data
{
    enum Flags
    {
        StagingArea = 1,
    };

    Data():
        path(),
        tags(new TagData),
        m_observers(),
        hash(),
        m_thumbnail(),
        m_flags(0),
        m_loadedData()
    {

    }

    Data(const Data &) = delete;
    Data& operator=(const Data &) = delete;

    std::string path;
    std::shared_ptr<ITagData> tags;
    std::set<IObserver *> m_observers;
    ThreadSafeResource<PhotoInfo::Hash> hash;
    ThreadSafeResource<QPixmap> m_thumbnail;
    ThreadSafeResource<unsigned int> m_flags;

    struct LoadedData
    {
        LoadedData(): m_thumbnail(false), m_hash(false) {}

        bool m_thumbnail;
        bool m_hash;
    } m_loadedData;
};


PhotoInfo::PhotoInfo(const std::string &p): m_data(new Data)
{
    std::unique_ptr<ITagData> p_tags = TagFeederFactory::get()->getTagsFor(p);

    m_data->tags = std::move(p_tags);
    m_data->path = p;
}


PhotoInfo::PhotoInfo(const APhotoInfoInitData& init): m_data(new Data)
{
    //TODO: run hash to verify data consistency?

    m_data->path = init.path;
    m_data->tags = init.tags;

    setHash(init.hash);
}


PhotoInfo::~PhotoInfo()
{

}


const std::string& PhotoInfo::getPath() const
{
    return m_data->path;
}


std::shared_ptr<ITagData> PhotoInfo::getTags() const
{
    return m_data->tags;
}


const QPixmap& PhotoInfo::getThumbnail() const
{
    auto result = m_data->m_thumbnail.lock();

    return result.get();
}


const PhotoInfo::Hash& PhotoInfo::getHash() const
{
    assert(isHashLoaded());
    auto result = m_data->hash.lock();

    return result.get();
}


bool PhotoInfo::isLoaded() const
{
    return isHashLoaded() && isThumbnailLoaded();
}


bool PhotoInfo::isHashLoaded() const
{
    return m_data->m_loadedData.m_hash;
}


bool PhotoInfo::isThumbnailLoaded() const
{
    return m_data->m_loadedData.m_thumbnail;
}


void PhotoInfo::registerObserver(IObserver* observer)
{
    m_data->m_observers.insert(observer);
}


void PhotoInfo::unregisterObserver(IObserver* observer)
{
    m_data->m_observers.erase(observer);
}



void PhotoInfo::updated()
{
    for(IObserver* observer: m_data->m_observers)
        observer->photoUpdated();
}


void PhotoInfo::setHash(const Hash& hash)
{
    m_data->hash.lock().get() = hash;
    m_data->m_loadedData.m_hash = true;

    updated();
}


void PhotoInfo::setThumbnail(const QPixmap& thumbnail)
{
    m_data->m_thumbnail.lock().get() = thumbnail;
    m_data->m_loadedData.m_thumbnail = true;

    updated();
}


void PhotoInfo::setTemporaryThumbnail(const QPixmap& thumbnail)
{
    m_data->m_thumbnail.lock().get() = thumbnail;

    updated();
}


void PhotoInfo::markStagingArea(bool on)
{
    auto flags = m_data->m_flags.lock();

    if (on)
        flags.get() |= Data::StagingArea;
    else
        flags.get() &= ~Data::StagingArea;
}


APhotoInfoInitData::APhotoInfoInitData(): path(), tags(new TagData), hash()
{

}
