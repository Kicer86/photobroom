
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

PhotoInfo::PhotoInfo(const Photo::DataDelta& dataDelta, IPhotoInfoStorekeeper* storekeeper):
    m_data(nullptr),
    m_storekeeper(storekeeper)
{
    assert(dataDelta.id.valid());

    m_data = std::make_unique<Data>();
    auto data = m_data->m_data.lock();

    data->id = dataDelta.id;

    if (dataDelta.has(Photo::Field::Checksum))
        data->sha256Sum = dataDelta.getAs<Photo::Sha256sum>(Photo::Field::Checksum);

    if (dataDelta.has(Photo::Field::Flags))
        data->flags = dataDelta.getAs<Photo::FlagValues>(Photo::Field::Flags);

    if (dataDelta.has(Photo::Field::Geometry))
        data->geometry = dataDelta.getAs<QSize>(Photo::Field::Geometry);

    if (dataDelta.has(Photo::Field::GroupInfo))
        data->groupInfo = dataDelta.getAs<GroupInfo>(Photo::Field::GroupInfo);

    if (dataDelta.has(Photo::Field::Path))
        data->path = dataDelta.getAs<QString>(Photo::Field::Path);

    if (dataDelta.has(Photo::Field::Tags))
        data->tags = dataDelta.getAs<Tag::TagsList>(Photo::Field::Tags);
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


void PhotoInfo::setSha256(const Photo::Sha256sum& sha256)
{
    assert(isSha256Loaded() == false);

    auto data = m_data->m_data.lock();
    data->sha256Sum = sha256;
    data->flags[Photo::FlagsE::Sha256Loaded] = 1;

    Photo::DataDelta delta;
    delta.id = data->id;
    delta.data[Photo::Field::Checksum] = data->sha256Sum;
    delta.data[Photo::Field::Flags] = data->flags;
    m_storekeeper->update(delta);
}


void PhotoInfo::setGeometry(const QSize& geometry)
{
    assert(isGeometryLoaded() == false);

    auto data = m_data->m_data.lock();
    data->geometry = geometry;
    data->flags[Photo::FlagsE::GeometryLoaded] = 1;

    Photo::DataDelta delta;
    delta.id = data->id;
    delta.data[Photo::Field::Geometry] = data->geometry;
    delta.data[Photo::Field::Flags] = data->flags;
    m_storekeeper->update(delta);
}


void PhotoInfo::setTags(const Tag::TagsList& tags)
{
    auto data = m_data->m_data.lock();
    data->tags = tags;

    Photo::DataDelta delta;
    delta.id = data->id;
    delta.data[Photo::Field::Tags] = data->tags;
    m_storekeeper->update(delta);
}


void PhotoInfo::setTag(const TagNameInfo& name, const TagValue& value)
{
    auto data = m_data->m_data.lock();
    data->tags[name] = value;

    Photo::DataDelta delta;
    delta.id = data->id;
    delta.data[Photo::Field::Tags] = data->tags;
    m_storekeeper->update(delta);
}


void PhotoInfo::setGroup(const GroupInfo& info)
{
    auto data = m_data->m_data.lock();
    data->groupInfo = info;

    Photo::DataDelta delta;
    delta.id = data->id;
    delta.data[Photo::Field::GroupInfo] = data->groupInfo;
    m_storekeeper->update(delta);
}


void PhotoInfo::markFlag(Photo::FlagsE flag, int v)
{
    m_data->m_data.lock()->flags[flag] = v;

    auto data = m_data->m_data.lock();
    data->flags[flag] = v;

    Photo::DataDelta delta;
    delta.id = data->id;
    delta.data[Photo::Field::Flags] = data->flags;
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
