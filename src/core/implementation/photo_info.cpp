
#include "photo_info.hpp"

#include <memory>
#include <mutex>

#include <QPixmap>

#include <OpenLibrary/palgorithm/ts_resource.hpp>

#include "tag.hpp"
#include "itagfeeder.hpp"
#include "task_executor.hpp"


PhotoInfo::Id::Id(): m_value(0), m_valid(false)
{

}


PhotoInfo::Id::Id(PhotoInfo::Id::type v): m_value(v), m_valid(true)
{

}


bool PhotoInfo::Id::operator!() const
{
    return !m_valid;
}


PhotoInfo::Id::operator PhotoInfo::Id::type() const
{
    return m_value;
}


bool PhotoInfo::Id::valid() const
{
    return m_valid;
}


PhotoInfo::Id::type PhotoInfo::Id::value() const
{
    return m_value;
}


/*********************************************************************************************************/


PhotoInfo::Flags::Flags(): stagingArea(false), tagsLoaded(false), hashLoaded(false), thumbnailLoaded(false)
{

}


APhotoInfoInitData::APhotoInfoInitData(): path(), tags(new TagData), hash()
{

}


struct PhotoInfo::Data
{
    Data():
        path(),
        tags(new TagData),
        m_observers(),
        hash(),
        m_thumbnail(),
        m_flags(),
        m_id()
    {

    }

    Data(const Data &) = delete;
    Data& operator=(const Data &) = delete;

    QString path;
    std::shared_ptr<ITagData> tags;
    std::set<IObserver *> m_observers;
    ThreadSafeResource<PhotoInfo::Hash> hash;
    ThreadSafeResource<QPixmap> m_thumbnail;
    ThreadSafeResource<PhotoInfo::Flags> m_flags;
    ThreadSafeResource<PhotoInfo::Id> m_id;
};


PhotoInfo::PhotoInfo(const QString &p): m_data(new Data)
{
    std::unique_ptr<ITagData> p_tags = TagFeederFactory::get()->getTagsFor(p);

    m_data->tags = std::move(p_tags);
    m_data->path = p;

    QPixmap tmpThumbnail;
    tmpThumbnail.load(":/core/images/clock.svg");             //use temporary thumbnail until final one is ready
    m_data->m_thumbnail.lock().get() = tmpThumbnail;
}


PhotoInfo::PhotoInfo(const APhotoInfoInitData& init): m_data(new Data)
{
    //TODO: run hash to verify data consistency?

    m_data->path = init.path;
    m_data->tags = init.tags;

    initHash(init.hash);
}


PhotoInfo::~PhotoInfo()
{

}


const QString& PhotoInfo::getPath() const
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


PhotoInfo::Id PhotoInfo::getID() const
{
    PhotoInfo::Id result = m_data->m_id.lock().get();

    return result;
}


bool PhotoInfo::isLoaded() const
{
    return isHashLoaded() && isThumbnailLoaded();
}


bool PhotoInfo::isHashLoaded() const
{
    return getFlags().hashLoaded;
}


bool PhotoInfo::isThumbnailLoaded() const
{
    return getFlags().thumbnailLoaded;
}


bool PhotoInfo::areTagsLoaded() const
{
    return getFlags().tagsLoaded;
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


void PhotoInfo::initHash(const Hash& hash)
{
    m_data->hash.lock().get() = hash;
    m_data->m_flags.lock().get().hashLoaded = true;

    updated();
}


void PhotoInfo::initThumbnail(const QPixmap& thumbnail)
{
    m_data->m_thumbnail.lock().get() = thumbnail;
    m_data->m_flags.lock().get().thumbnailLoaded = true;

    updated();
}


void PhotoInfo::initID(const PhotoInfo::Id& id)
{
    m_data->m_id.lock().get() = id;
}


void PhotoInfo::markStagingArea(bool on)
{
    m_data->m_flags.lock().get().stagingArea = on;

    updated();
}


PhotoInfo::Flags PhotoInfo::getFlags() const
{
    Flags result = m_data->m_flags.lock().get();

    return result;
}

