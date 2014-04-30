
#include "aphoto_info.hpp"

#include <memory>
#include <mutex>

#include <QPixmap>

#include "tag.hpp"
#include "itagfeeder.hpp"
#include "task_executor.hpp"


struct PhotoInfo::Data
{
    Data(const std::string &p): path(p), tags(), hash(), hashMutex(), m_observer(nullptr)
    {
        std::unique_ptr<ITagData> p_tags = TagFeederFactory::get()->getTagsFor(p);

        tags = std::move(p_tags);
    }

    Data(const APhotoInfoInitData& initData):
        path(initData.path),
        tags(initData.tags),
        hash(initData.hash),
        hashMutex(),
        m_observer(nullptr)
    {
    }

    Data(const Data& other):
        path(other.path),
        tags(other.tags),
        hash(other.hash),
        hashMutex(),
        m_observer(other.m_observer)
    {}

    Data& operator=(const Data &) = delete;

    std::string path;
    std::shared_ptr<ITagData> tags;
    PhotoInfo::Hash hash;
    std::mutex hashMutex;
    IObserver* m_observer;
    QPixmap m_thumbnail;
};


PhotoInfo::PhotoInfo(const std::string &p): m_data(new Data(p))
{
    //auto task = std::make_shared<HashAssigner>(this);     //calculate hash of 'this'
    //TaskExecutorConstructor::get()->add(task);
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
    return m_data->m_thumbnail;
}


const PhotoInfo::Hash& PhotoInfo::getHash() const
{
    //hash may be simultaneously read and write, protect it
    std::unique_lock<std::mutex> lock(m_data->hashMutex);

    //assert(m_data->hash.empty() == false);
    return m_data->hash;
}


void PhotoInfo::registerObserver(IObserver* observer)
{
    assert(m_data->m_observer == nullptr);
    m_data->m_observer = observer;
}


void PhotoInfo::updated()
{
    if (m_data->m_observer != nullptr)
        m_data->m_observer->photoUpdated();
}


void PhotoInfo::setHash(const Hash& hash)
{
    //hash may be simultaneously read and write, protect it
    std::unique_lock<std::mutex> lock(m_data->hashMutex);

    assert(m_data->hash.empty());
    m_data->hash = hash;

    lock.unlock(); //write done

    updated();
}


void PhotoInfo::setThumbnail(const QPixmap& thumbnail)
{
    //TODO: mutex
    m_data->m_thumbnail = thumbnail;

    updated();
}


APhotoInfoInitData::APhotoInfoInitData(): path(), tags(new TagData), hash()
{

}

