
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
    ol::ThreadSafeResource<Database::Sha256sum> sha256;
    ol::ThreadSafeResource<QImage> m_thumbnail;
    ol::ThreadSafeResource<IPhotoInfo::Flags> m_flags;
    ol::ThreadSafeResource<Database::Id> m_id;
    bool m_valid;
};


PhotoInfo::PhotoInfo(const QString &p): m_data(new Data)
{
    m_data->path = p;

    //default values
    QImage tmpThumbnail;
    tmpThumbnail.load(":/core/images/clock.svg");             //use temporary thumbnail until final one is ready
    m_data->m_thumbnail.lock().get() = tmpThumbnail;

    markFlag(Database::FlagsE::StagingArea, 1);
}


PhotoInfo::PhotoInfo(const Database::PhotoData& data): m_data(new Data)
{
    *m_data->m_id.lock() = data.id;
    *m_data->sha256.lock() = data.sha256Sum;
    *m_data->tags.lock() = data.tags;

    auto flags = m_data->m_flags.lock().get();
    flags.exifLoaded = data.getFlag(Database::FlagsE::ExifLoaded);
    flags.sha256Loaded = data.getFlag(Database::FlagsE::Sha256Loaded);
    flags.stagingArea = data.getFlag(Database::FlagsE::StagingArea);
    flags.thumbnailLoaded = data.getFlag(Database::FlagsE::ThumbnailLoaded);

    m_data->path = data.path;
    *m_data->m_thumbnail.lock() = data.thumbnail;
}


PhotoInfo::~PhotoInfo()
{

}


Database::PhotoData PhotoInfo::data() const
{
    Database::PhotoData d;
    d.id = getID();
    d.sha256Sum = getSha256();
    d.tags = getTags();
    d.flags[Database::FlagsE::ExifLoaded] = getFlag(Database::FlagsE::ExifLoaded);
    d.flags[Database::FlagsE::Sha256Loaded] = getFlag(Database::FlagsE::Sha256Loaded);
    d.flags[Database::FlagsE::StagingArea] = getFlag(Database::FlagsE::StagingArea);
    d.flags[Database::FlagsE::ThumbnailLoaded] = getFlag(Database::FlagsE::ThumbnailLoaded);
    d.path = getPath();
    d.thumbnail = getThumbnail();
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


const Database::Sha256sum& PhotoInfo::getSha256() const
{
    assert(isSha256Loaded());
    auto result = m_data->sha256.lock();

    return result.get();
}


Database::Id PhotoInfo::getID() const
{
    Database::Id result = m_data->m_id.lock().get();

    return result;
}


bool PhotoInfo::isFullyInitialized() const
{
    return isSha256Loaded() && isThumbnailLoaded() && isExifDataLoaded();
}


bool PhotoInfo::isSha256Loaded() const
{
    return getFlag(Database::FlagsE::Sha256Loaded) > 0;
}


bool PhotoInfo::isThumbnailLoaded() const
{
    return getFlag(Database::FlagsE::ThumbnailLoaded) > 0;
}


bool PhotoInfo::isExifDataLoaded() const
{
    return getFlag(Database::FlagsE::ExifLoaded) > 0;
}


void PhotoInfo::registerObserver(IObserver* observer)
{
    m_data->m_observers.insert(observer);
}


void PhotoInfo::unregisterObserver(IObserver* observer)
{
    m_data->m_observers.erase(observer);
}


void PhotoInfo::initSha256(const Database::Sha256sum& sha256)
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


void PhotoInfo::initID(const Database::Id& id)
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


void PhotoInfo::markFlag(Database::FlagsE flag, int v)
{
    switch (flag)
    {
        case Database::FlagsE::StagingArea:
            m_data->m_flags.lock()->stagingArea = v;
            break;

        case Database::FlagsE::ExifLoaded:
            m_data->m_flags.lock()->exifLoaded = v;
            break;

        case Database::FlagsE::Sha256Loaded:
            m_data->m_flags.lock()->sha256Loaded = v;
            break;

        case Database::FlagsE::ThumbnailLoaded:
            m_data->m_flags.lock()->thumbnailLoaded = v;
            break;
    }

    updated();
}


int PhotoInfo::getFlag(Database::FlagsE flag) const
{
    int result = 0;

    switch (flag)
    {
        case Database::FlagsE::StagingArea:
            result = m_data->m_flags.lock()->stagingArea;
            break;

        case Database::FlagsE::ExifLoaded:
            result = m_data->m_flags.lock()->exifLoaded;
            break;

        case Database::FlagsE::Sha256Loaded:
            result = m_data->m_flags.lock()->sha256Loaded;
            break;

        case Database::FlagsE::ThumbnailLoaded:
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
