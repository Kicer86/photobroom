
#include "aphoto_info.hpp"

#include <memory>
#include <mutex>

#include <QPixmap>

#include "tag.hpp"
#include "itagfeeder.hpp"
#include "task_executor.hpp"


struct APhotoInfo::Data
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
    APhotoInfo::Hash hash;
    std::mutex hashMutex;
    IObserver* m_observer;
    QPixmap m_thumbnail;
};


APhotoInfo::APhotoInfo(const std::string &p): m_data(new Data(p))
{
    //auto task = std::make_shared<HashAssigner>(this);     //calculate hash of 'this'
    //TaskExecutorConstructor::get()->add(task);
}


APhotoInfo::APhotoInfo(const APhotoInfoInitData& init): m_data(new Data(init))
{
    //TODO: run hash to verify data consistency?
}


APhotoInfo::~APhotoInfo()
{

}


const std::string& APhotoInfo::getPath() const
{
    return m_data->path;
}


std::shared_ptr<ITagData> APhotoInfo::getTags() const
{
    return m_data->tags;
}


const QPixmap& APhotoInfo::getThumbnail() const
{
    return m_data->m_thumbnail;
}


const APhotoInfo::Hash& APhotoInfo::getHash() const
{
    //hash may be simultaneously read and write, protect it
    std::unique_lock<std::mutex> lock(m_data->hashMutex);

    //assert(m_data->hash.empty() == false);
    return m_data->hash;
}


void APhotoInfo::registerObserver(IObserver* observer)
{
    assert(m_data->m_observer == nullptr);
    m_data->m_observer = observer;
}


void APhotoInfo::updated()
{
    if (m_data->m_observer != nullptr)
        m_data->m_observer->photoUpdated();
}


void APhotoInfo::setHash(const Hash& hash)
{
    //hash may be simultaneously read and write, protect it
    std::unique_lock<std::mutex> lock(m_data->hashMutex);

    assert(m_data->hash.empty());
    m_data->hash = hash;

    lock.unlock(); //write done

    updated();
}


void APhotoInfo::setThumbnail(const QPixmap& thumbnail)
{
    //TODO: mutex
    m_data->m_thumbnail = thumbnail;

    updated();
}


APhotoInfoInitData::APhotoInfoInitData(): path(), tags(new TagData), hash()
{

}

