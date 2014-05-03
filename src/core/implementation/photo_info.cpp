
#include "photo_info.hpp"

#include <memory>
#include <mutex>

#include <QPixmap>

#include "tag.hpp"
#include "itagfeeder.hpp"
#include "task_executor.hpp"


struct PhotoInfo::Data
{
    Data(const std::string &p):
        path(p),
        tags(),
        hash(),
        hashMutex(),
        thumbnailMutex(),
        m_observer(),
        m_thumbnail(),
        m_loadedData()
    {
        std::unique_ptr<ITagData> p_tags = TagFeederFactory::get()->getTagsFor(p);

        tags = std::move(p_tags);
    }

    Data(const APhotoInfoInitData& initData):
        path(initData.path),
        tags(initData.tags),
        hash(initData.hash),
        hashMutex(),
        thumbnailMutex(),
        m_observer(),
        m_thumbnail(),
        m_loadedData()
    {
    }

    Data(const Data& other):
        path(other.path),
        tags(other.tags),
        hash(other.hash),
        hashMutex(),
        thumbnailMutex(),
        m_observer(other.m_observer),
        m_thumbnail(),
        m_loadedData()
    {}

    Data& operator=(const Data &) = delete;

    std::string path;
    std::shared_ptr<ITagData> tags;
    PhotoInfo::Hash hash;
    std::mutex hashMutex;
    std::mutex thumbnailMutex;
    std::set<IObserver *> m_observer;
    QPixmap m_thumbnail;

    struct LoadedData
    {
        LoadedData(): m_thumbnail(false), m_hash(false) {}

        bool m_thumbnail;
        bool m_hash;
    } m_loadedData;
};


PhotoInfo::PhotoInfo(const std::string &p): m_data(new Data(p))
{

}


PhotoInfo::PhotoInfo(const APhotoInfoInitData& init): m_data(new Data(init))
{
    //TODO: run hash to verify data consistency?
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
    return isHashLoaded() && m_data->m_loadedData.m_thumbnail;
}


bool PhotoInfo::isHashLoaded() const
{
    return m_data->m_loadedData.m_hash;
}


void PhotoInfo::registerObserver(IObserver* observer)
{
    m_data->m_observer.insert(observer);
}


void PhotoInfo::unregisterObserver(IObserver* observer)
{
    m_data->m_observer.erase(observer);
}



void PhotoInfo::updated()
{
    for(IObserver* observer: m_data->m_observer)
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


APhotoInfoInitData::APhotoInfoInitData(): path(), tags(new TagData), hash()
{

}
