
#include "backends/photo_info.hpp"

#include <memory>
#include <mutex>

#include <QPixmap>

#include <OpenLibrary/putils/ts_resource.hpp>

#include <core/tag.hpp>
#include <core/task_executor.hpp>


PhotoInfo::Id::Id(): m_value(-1), m_valid(false)
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
        tags(),
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

    ol::ThreadSafeResource<Tag::TagsList> tags;
    ol::ThreadSafeResource<PhotoInfo::Hash> hash;
    ol::ThreadSafeResource<QPixmap> m_thumbnail;
    ol::ThreadSafeResource<PhotoInfo::Flags> m_flags;
    ol::ThreadSafeResource<PhotoInfo::Id> m_id;
};


PhotoInfo::PhotoInfo(const QString &p): m_data(new Data)
{
    m_data->path = p;

    //default values
    QPixmap tmpThumbnail;
    tmpThumbnail.load(":/core/images/clock.svg");             //use temporary thumbnail until final one is ready
    m_data->m_thumbnail.lock().get() = tmpThumbnail;

    markStagingArea(true);
}


PhotoInfo::~PhotoInfo()
{

}


const QString& PhotoInfo::getPath() const
{
    return m_data->path;
}


const Tag::TagsList& PhotoInfo::getTags() const
{
    auto result = m_data->tags.lock();

    return result.get();
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


ol::ThreadSafeResource<Tag::TagsList>::Accessor PhotoInfo::accessTags()
{
    auto result = m_data->tags.lock(this);

    return std::move(result);
}


void PhotoInfo::setTags(const Tag::TagsList& tags)
{
    m_data->tags.lock().get() = tags;

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


void PhotoInfo::markExifDataLoaded(bool on)
{
    m_data->m_flags.lock()->exifLoaded = on;

    updated();
}


void PhotoInfo::updated()
{
    for(IObserver* observer: m_data->m_observers)
        observer->photoUpdated(this);
}


void PhotoInfo::unlocked()
{
    updated();
}
