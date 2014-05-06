
#include "photo_info.hpp"

#include <memory>
#include <mutex>

#include <QPixmap>

#include "tag.hpp"
#include "itagfeeder.hpp"
#include "task_executor.hpp"


struct PhotoInfo::Data
{
    //TODO: remove constructors 'Data' is meant to be POD
    Data():
        path(),
        tags(new TagData),
        hash(),
        hashMutex(),
        thumbnailMutex(),
        m_observers(),
        m_thumbnail(),
        m_loadedData()
    {

    }

    Data(const Data &) = delete;
    Data& operator=(const Data &) = delete;

    std::string path;
    std::shared_ptr<ITagData> tags;
    PhotoInfo::Hash hash;
    std::mutex hashMutex;
    std::mutex thumbnailMutex;
    std::set<IObserver *> m_observers;
    QPixmap m_thumbnail;

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
    //thumbnail may be simultaneously read and write, protect it
    std::unique_lock<std::mutex> lock(m_data->thumbnailMutex);

    return m_data->m_thumbnail;
}


const PhotoInfo::Hash& PhotoInfo::getHash() const
{
    //hash may be simultaneously read and write, protect it
    std::unique_lock<std::mutex> lock(m_data->hashMutex);

    assert(isHashLoaded());
    return m_data->hash;
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
    //hash may be simultaneously read and write, protect it
    std::unique_lock<std::mutex> lock(m_data->hashMutex);

    assert(m_data->hash.empty());
    m_data->hash = hash;
    m_data->m_loadedData.m_hash = true;

    lock.unlock(); //write done

    updated();
}


void PhotoInfo::setThumbnail(const QPixmap& thumbnail)
{
    //thumnail may be simultaneously read and write, protect it
    std::unique_lock<std::mutex> lock(m_data->hashMutex);

    m_data->m_thumbnail = thumbnail;
    m_data->m_loadedData.m_thumbnail = true;

    lock.unlock(); //write done

    updated();
}


void PhotoInfo::setTemporaryThumbnail(const QPixmap& thumbnail)
{
    //thumnail may be simultaneously read and write, protect it
    std::unique_lock<std::mutex> lock(m_data->hashMutex);

    m_data->m_thumbnail = thumbnail;

    lock.unlock(); //write done

    updated();
}


void PhotoInfo::markStagingArea(bool)
{

}


APhotoInfoInitData::APhotoInfoInitData(): path(), tags(new TagData), hash()
{

}
