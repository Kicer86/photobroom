
#include "photo_info.hpp"

#include <memory>
#include <mutex>

#include <QPixmap>

#include <OpenLibrary/palgorithm/ts_resource.hpp>

#include "tag.hpp"
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


PhotoInfo::Flags::Flags(): stagingArea(false), exifLoaded(false), hashLoaded(false), thumbnailLoaded(false)
{

}


struct PhotoInfo::Data
{
    Data():
        path(),
        m_observers(),
        tags(new TagData),
        hash(),
        m_thumbnail(),
        m_flags(),
        m_id()
    {

    }

    Data(const Data &) = delete;
    Data& operator=(const Data &) = delete;

    QString path;
    std::set<IObserver *> m_observers;

    std::shared_ptr<ITagData> tags;
    ThreadSafeResource<PhotoInfo::Hash> hash;
    ThreadSafeResource<QPixmap> m_thumbnail;
    ThreadSafeResource<PhotoInfo::Flags> m_flags;
    ThreadSafeResource<PhotoInfo::Id> m_id;
};


PhotoInfo::PhotoInfo(const QString &p): m_data(new Data)
{
    m_data->path = p;

    QPixmap tmpThumbnail;
    tmpThumbnail.load(":/core/images/clock.svg");             //use temporary thumbnail until final one is ready
    m_data->m_thumbnail.lock().get() = tmpThumbnail;
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


bool PhotoInfo::isFullyInitialized() const
{
    return isHashLoaded() && isThumbnailLoaded() && isExifDataLoaded();
}


bool PhotoInfo::isHashLoaded() const
{
    return getFlags().hashLoaded;
}


bool PhotoInfo::isThumbnailLoaded() const
{
    return getFlags().thumbnailLoaded;
}


bool PhotoInfo::isExifDataLoaded() const
{
    return getFlags().exifLoaded;
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
    assert(isHashLoaded() == false);

    m_data->hash.lock().get() = hash;
    m_data->m_flags.lock().get().hashLoaded = true;

    updated();
}


void PhotoInfo::initThumbnail(const QPixmap& thumbnail)
{
    assert(isThumbnailLoaded() == false);

    m_data->m_thumbnail.lock().get() = thumbnail;
    m_data->m_flags.lock().get().thumbnailLoaded = true;

    updated();
}


void PhotoInfo::initID(const PhotoInfo::Id& id)
{
    *m_data->m_id.lock() = id;
}


void PhotoInfo::initExifData(std::unique_ptr<ITagData >&& tags)
{
    assert(isExifDataLoaded() == false);

    m_data->tags = std::move(tags);
    m_data->m_flags.lock()->exifLoaded = true;

    updated();
}


void PhotoInfo::markStagingArea(bool on)
{
    m_data->m_flags.lock()->stagingArea = on;

    updated();
}


PhotoInfo::Flags PhotoInfo::getFlags() const
{
    Flags result = *m_data->m_flags.lock();

    return result;
}

