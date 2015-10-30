
#include "photo_info.hpp"

#include <memory>
#include <mutex>

#include <QImage>

#include <OpenLibrary/putils/ts_resource.hpp>

#include <core/tag.hpp>
#include <core/task_executor.hpp>

#include "photo_data.hpp"


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
        m_id(),
        m_valid(true)
    {
    }

    Data(const Data &) = delete;
    Data& operator=(const Data &) = delete;

    QString path;
    std::set<IObserver *> m_observers;

    ol::ThreadSafeResource<Tag::TagsList> tags;
    ol::ThreadSafeResource<Photo::Sha256sum> sha256;
    ol::ThreadSafeResource<QImage> m_thumbnail;
    ol::ThreadSafeResource<IPhotoInfo::Flags> m_flags;
    ol::ThreadSafeResource<Photo::Id> m_id;
    bool m_valid;
};


PhotoInfo::PhotoInfo(const QString &p): m_data(new Data)
{
    m_data->path = p;

    //default values
    QImage tmpThumbnail;
    tmpThumbnail.load(":/core/images/clock.svg");             //use temporary thumbnail until final one is ready
    m_data->m_thumbnail.lock().get() = tmpThumbnail;

    markFlag(Photo::FlagsE::StagingArea, 1);
}


PhotoInfo::PhotoInfo(const Photo::Data& data): m_data(new Data)
{
    *m_data->m_id.lock() = data.id;
    *m_data->sha256.lock() = data.sha256Sum;
    *m_data->tags.lock() = data.tags;

    IPhotoInfo::Flags& flags = m_data->m_flags.lock().get();
    flags.exifLoaded = data.getFlag(Photo::FlagsE::ExifLoaded);
    flags.sha256Loaded = data.getFlag(Photo::FlagsE::Sha256Loaded);
    flags.stagingArea = data.getFlag(Photo::FlagsE::StagingArea);
    flags.thumbnailLoaded = data.getFlag(Photo::FlagsE::ThumbnailLoaded);

    m_data->path = data.path;
    *m_data->m_thumbnail.lock() = data.thumbnail;
}


PhotoInfo::~PhotoInfo()
{

}


Photo::Data PhotoInfo::data() const
{
    Photo::Data d;
    d.id = getID();
    d.sha256Sum = getSha256();
    d.tags = getTags();
    d.flags[Photo::FlagsE::ExifLoaded] = getFlag(Photo::FlagsE::ExifLoaded);
    d.flags[Photo::FlagsE::Sha256Loaded] = getFlag(Photo::FlagsE::Sha256Loaded);
    d.flags[Photo::FlagsE::StagingArea] = getFlag(Photo::FlagsE::StagingArea);
    d.flags[Photo::FlagsE::ThumbnailLoaded] = getFlag(Photo::FlagsE::ThumbnailLoaded);
    d.path = getPath();
    d.thumbnail = getThumbnail();

    return d;
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


const QImage& PhotoInfo::getThumbnail() const
{
    auto result = m_data->m_thumbnail.lock();

    return result.get();
}


const Photo::Sha256sum& PhotoInfo::getSha256() const
{
    assert(isSha256Loaded());
    auto result = m_data->sha256.lock();

    return result.get();
}


Photo::Id PhotoInfo::getID() const
{
    Photo::Id result = m_data->m_id.lock().get();

    return result;
}


bool PhotoInfo::isFullyInitialized() const
{
    return isSha256Loaded() && isThumbnailLoaded() && isExifDataLoaded();
}


bool PhotoInfo::isSha256Loaded() const
{
    return getFlag(Photo::FlagsE::Sha256Loaded) > 0;
}


bool PhotoInfo::isThumbnailLoaded() const
{
    return getFlag(Photo::FlagsE::ThumbnailLoaded) > 0;
}


bool PhotoInfo::isExifDataLoaded() const
{
    return getFlag(Photo::FlagsE::ExifLoaded) > 0;
}


void PhotoInfo::registerObserver(IObserver* observer)
{
    m_data->m_observers.insert(observer);
}


void PhotoInfo::unregisterObserver(IObserver* observer)
{
    m_data->m_observers.erase(observer);
}


void PhotoInfo::initSha256(const Photo::Sha256sum& sha256)
{
    assert(isSha256Loaded() == false);

    m_data->sha256.lock().get() = sha256;
    m_data->m_flags.lock().get().sha256Loaded = true;

    updated();
}


void PhotoInfo::initThumbnail(const QImage& thumbnail)
{
    assert(isThumbnailLoaded() == false);

    m_data->m_thumbnail.lock().get() = thumbnail;
    m_data->m_flags.lock().get().thumbnailLoaded = true;

    updated();
}


void PhotoInfo::initID(const Photo::Id& id)
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


void PhotoInfo::markFlag(Photo::FlagsE flag, int v)
{
    switch (flag)
    {
        case Photo::FlagsE::StagingArea:
            m_data->m_flags.lock()->stagingArea = v;
            break;

        case Photo::FlagsE::ExifLoaded:
            m_data->m_flags.lock()->exifLoaded = v;
            break;

        case Photo::FlagsE::Sha256Loaded:
            m_data->m_flags.lock()->sha256Loaded = v;
            break;

        case Photo::FlagsE::ThumbnailLoaded:
            m_data->m_flags.lock()->thumbnailLoaded = v;
            break;
    }

    updated();
}


int PhotoInfo::getFlag(Photo::FlagsE flag) const
{
    int result = 0;

    switch (flag)
    {
        case Photo::FlagsE::StagingArea:
            result = m_data->m_flags.lock()->stagingArea;
            break;

        case Photo::FlagsE::ExifLoaded:
            result = m_data->m_flags.lock()->exifLoaded;
            break;

        case Photo::FlagsE::Sha256Loaded:
            result = m_data->m_flags.lock()->sha256Loaded;
            break;

        case Photo::FlagsE::ThumbnailLoaded:
            result = m_data->m_flags.lock()->thumbnailLoaded;
            break;
    }

    return result;
}


void PhotoInfo::invalidate()
{
    m_data->m_valid = false;
}


bool PhotoInfo::isValid()
{
    return m_data->m_valid;
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
