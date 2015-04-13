
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


PhotoInfo::Flags::Flags(): stagingArea(0), exifLoaded(0), sha256Loaded(0), thumbnailLoaded(0)
{

}


struct PhotoInfo::Data
{
    Data():
        path(),
        m_observers(),
        tags(),
        sha256(),
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
    ol::ThreadSafeResource<PhotoInfo::Sha256sum> sha256;
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

    markFlag(FlagsE::StagingArea, 1);
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


const PhotoInfo::Sha256sum& PhotoInfo::getSha256() const
{
    assert(isSha256Loaded());
    auto result = m_data->sha256.lock();

    return result.get();
}


PhotoInfo::Id PhotoInfo::getID() const
{
    PhotoInfo::Id result = m_data->m_id.lock().get();

    return result;
}


bool PhotoInfo::isFullyInitialized() const
{
    return isSha256Loaded() && isThumbnailLoaded() && isExifDataLoaded();
}


bool PhotoInfo::isSha256Loaded() const
{
    return getFlag(FlagsE::Sha256Loaded) > 0;
}


bool PhotoInfo::isThumbnailLoaded() const
{
    return getFlag(FlagsE::ThumbnailLoaded) > 0;
}


bool PhotoInfo::isExifDataLoaded() const
{
    return getFlag(FlagsE::ExifLoaded) > 0;
}


void PhotoInfo::registerObserver(IObserver* observer)
{
    m_data->m_observers.insert(observer);
}


void PhotoInfo::unregisterObserver(IObserver* observer)
{
    m_data->m_observers.erase(observer);
}


void PhotoInfo::initSha256(const Sha256sum& sha256)
{
    assert(isSha256Loaded() == false);

    m_data->sha256.lock().get() = sha256;
    m_data->m_flags.lock().get().sha256Loaded = true;

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


void PhotoInfo::markFlag(IPhotoInfo::FlagsE flag, int v)
{
    switch (flag)
    {
        case FlagsE::StagingArea:
            m_data->m_flags.lock()->stagingArea = v;
            break;

        case FlagsE::ExifLoaded:
            m_data->m_flags.lock()->exifLoaded = v;
            break;

        case FlagsE::Sha256Loaded:
            m_data->m_flags.lock()->sha256Loaded = v;
            break;

        case FlagsE::ThumbnailLoaded:
            m_data->m_flags.lock()->thumbnailLoaded = v;
            break;
    }

    updated();
}


int PhotoInfo::getFlag(IPhotoInfo::FlagsE flag) const
{
    int result = 0;

    switch (flag)
    {
        case FlagsE::StagingArea:
            result = m_data->m_flags.lock()->stagingArea;
            break;

        case FlagsE::ExifLoaded:
            result = m_data->m_flags.lock()->exifLoaded;
            break;

        case FlagsE::Sha256Loaded:
            result = m_data->m_flags.lock()->sha256Loaded;
            break;

        case FlagsE::ThumbnailLoaded:
            result = m_data->m_flags.lock()->thumbnailLoaded;
            break;
    }

    return result;
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
