
#include "photo_info.hpp"

#include <memory>
#include <mutex>

#include <QImageReader>

#include <OpenLibrary/putils/ts_resource.hpp>

#include <core/tag.hpp>
#include <core/task_executor.hpp>

#include "photo_data.hpp"


struct PhotoInfo::Data
{
    Data():
        path(),
        m_observers(),
        m_data(),
        m_valid(true)
    {
    }

    Data(const Data &) = delete;
    Data& operator=(const Data &) = delete;

    ol::ThreadSafeResource<QString> path;
    ol::ThreadSafeResource<std::set<IObserver *>> m_observers;
    ol::ThreadSafeResource<Photo::Data> m_data;
    bool m_valid;
};


PhotoInfo::PhotoInfo(const QString &p): m_data(new Data)
{
    *m_data->path.lock() = p;

    const QImageReader reader(p);
    m_data->m_data.lock()->geometry = reader.size();

    markFlag(Photo::FlagsE::GeometryLoaded, 1);
    markFlag(Photo::FlagsE::StagingArea, 1);
}


PhotoInfo::PhotoInfo(const Photo::Data& data): m_data(new Data)
{
    *m_data->m_data.lock() = data;
}


PhotoInfo::~PhotoInfo()
{

}


Photo::Data PhotoInfo::data() const
{
    return m_data->m_data.lock().get();
}


const QString PhotoInfo::getPath() const
{
    return m_data->m_data.lock()->path;
}


const Tag::TagsList PhotoInfo::getTags() const
{
    return m_data->m_data.lock()->tags;
}


const QSize PhotoInfo::getGeometry() const
{
    return m_data->m_data.lock()->geometry;
}


const Photo::Sha256sum PhotoInfo::getSha256() const
{
    assert(isSha256Loaded());

    return m_data->m_data.lock()->sha256Sum;
}


Photo::Id PhotoInfo::getID() const
{
    return m_data->m_data.lock()->id;
}


bool PhotoInfo::isFullyInitialized() const
{
    return isSha256Loaded() && isExifDataLoaded() && isGeometryLoaded();
}


bool PhotoInfo::isSha256Loaded() const
{
    return getFlag(Photo::FlagsE::Sha256Loaded) > 0;
}


bool PhotoInfo::isGeometryLoaded() const
{
    return getFlag(Photo::FlagsE::GeometryLoaded) > 0;
}


bool PhotoInfo::isExifDataLoaded() const
{
    return getFlag(Photo::FlagsE::ExifLoaded) > 0;
}


void PhotoInfo::registerObserver(IObserver* observer)
{
    m_data->m_observers.lock()->insert(observer);
}


void PhotoInfo::unregisterObserver(IObserver* observer)
{
    m_data->m_observers.lock()->erase(observer);
}


void PhotoInfo::setSha256(const Photo::Sha256sum& sha256)
{
    assert(isSha256Loaded() == false);

    m_data->m_data.lock()->sha256Sum = sha256;
    markFlag(Photo::FlagsE::Sha256Loaded, 1);

    updated(ChangeReason::Sha256Updated);
}


void PhotoInfo::setGeometry(const QSize& geometry)
{
    assert(isGeometryLoaded() == false);

    m_data->m_data.lock()->geometry = geometry;
    markFlag(Photo::FlagsE::GeometryLoaded, 1);

    updated(ChangeReason::GeometryUpdated);
}


void PhotoInfo::setTags(const Tag::TagsList& tags)
{
    m_data->m_data.lock()->tags = tags;

    updated(ChangeReason::TagsUpdated);
}


void PhotoInfo::setTag(const TagNameInfo& name, const TagValue& value)
{
    m_data->m_data.lock()->tags[name] = value;

    updated(ChangeReason::TagsUpdated);
}


void PhotoInfo::setGroup(const Group::Id& id)
{
    m_data->m_data.lock()->group_id = id;

    updated(ChangeReason::GroupUpdated);
}


void PhotoInfo::markFlag(Photo::FlagsE flag, int v)
{
    m_data->m_data.lock()->flags[flag] = v;

    updated(ChangeReason::FlagsUpdated);
}


int PhotoInfo::getFlag(Photo::FlagsE flag) const
{
    auto flags = m_data->m_data.lock()->flags;
    auto it = flags.find(flag);
    const int result = it == flags.end()? 0: it->second;

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


void PhotoInfo::updated(ChangeReason reason)
{
    auto observers = m_data->m_observers.lock();

    for(IObserver* observer: observers.get())
        observer->photoUpdated(this, reason);
}
