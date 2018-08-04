
#include "photo_info.hpp"

#include <memory>
#include <mutex>

#include <QImageReader>

#include <OpenLibrary/putils/ts_resource.hpp>

#include <core/tag.hpp>
#include <core/task_executor.hpp>

#include "photo_data.hpp"
#include "iphoto_info_storekeeper.hpp"

struct PhotoInfo::Data
{
    Data():
        m_data(),
        m_valid(true)
    {
    }

    Data(const Data &) = delete;
    Data& operator=(const Data &) = delete;

    ol::ThreadSafeResource<Photo::Data> m_data;
    bool m_valid;
};


PhotoInfo::PhotoInfo(const Photo::Data& data, IPhotoInfoStorekeeper* storekeeper):
    m_data(nullptr),
    m_storekeeper(storekeeper)
{
    m_data = std::make_unique<Data>();
    *m_data->m_data.lock() = data;
}


PhotoInfo::~PhotoInfo()
{

}


Photo::Data PhotoInfo::data() const
{
    return m_data->m_data.lock().get();
}


QString PhotoInfo::getPath() const
{
    return m_data->m_data.lock()->path;
}


Tag::TagsList PhotoInfo::getTags() const
{
    return m_data->m_data.lock()->tags;
}


QSize PhotoInfo::getGeometry() const
{
    return m_data->m_data.lock()->geometry;
}


Photo::Sha256sum PhotoInfo::getSha256() const
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


void PhotoInfo::setSha256(const Photo::Sha256sum& sha256)
{
    assert(isSha256Loaded() == false);

    auto data = m_data->m_data.lock();
    data->sha256Sum = sha256;
    data->flags[Photo::FlagsE::Sha256Loaded] = 1;

    Photo::DataDelta delta(data->id);
    delta.insert(Photo::Field::Checksum, data->sha256Sum);
    delta.insert(Photo::Field::Flags,    data->flags);
    m_storekeeper->update(delta);
}


void PhotoInfo::setGeometry(const QSize& geometry)
{
    assert(isGeometryLoaded() == false);

    auto data = m_data->m_data.lock();
    data->geometry = geometry;
    data->flags[Photo::FlagsE::GeometryLoaded] = 1;

    Photo::DataDelta delta(data->id);
    delta.insert(Photo::Field::Geometry, data->geometry);
    delta.insert(Photo::Field::Flags,    data->flags);
    m_storekeeper->update(delta);
}


void PhotoInfo::setTags(const Tag::TagsList& tags)
{
    auto data = m_data->m_data.lock();
    data->tags = tags;

    Photo::DataDelta delta(data->id);
    delta.insert(Photo::Field::Tags, data->tags);
    m_storekeeper->update(delta);
}


void PhotoInfo::setTag(const TagNameInfo& name, const TagValue& value)
{
    auto data = m_data->m_data.lock();
    data->tags[name] = value;

    Photo::DataDelta delta(data->id);
    delta.insert(Photo::Field::Tags, data->tags);
    m_storekeeper->update(delta);
}


void PhotoInfo::setGroup(const GroupInfo& info)
{
    auto data = m_data->m_data.lock();
    data->groupInfo = info;

    Photo::DataDelta delta(data->id);
    delta.insert(Photo::Field::GroupInfo, data->groupInfo);
    m_storekeeper->update(delta);
}


void PhotoInfo::markFlag(Photo::FlagsE flag, int v)
{
    m_data->m_data.lock()->flags[flag] = v;

    auto data = m_data->m_data.lock();
    data->flags[flag] = v;

    Photo::DataDelta delta(data->id);
    delta.insert(Photo::Field::Flags, data->flags);
    m_storekeeper->update(delta);
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
