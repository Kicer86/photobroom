
#ifndef FAKE_PHOTO_INFO_HPP
#define FAKE_PHOTO_INFO_HPP

#include "database/iphoto_info.hpp"


class FakePhotoInfo final: public IPhotoInfo
{
public:
    explicit FakePhotoInfo(const Photo::Data& data):
        m_data(data)
    {
    }

    ~FakePhotoInfo() = default;

    Photo::Data data() const override
    {
        return m_data;
    }

    QString getPath() const override
    {
        return m_data.path;
    }

    Tag::TagsList getTags() const override
    {
        return m_data.tags;
    }

    QSize getGeometry() const override
    {
        return m_data.geometry;
    }

    Photo::Sha256sum getSha256() const override
    {
        return m_data.sha256Sum;
    }

    Photo::Id getID() const override
    {
        return m_data.id;
    }

    bool isFullyInitialized() const override
    {
        return isSha256Loaded() && isExifDataLoaded() && isGeometryLoaded();
    }
    bool isSha256Loaded() const override
    {
        return getFlag(Photo::FlagsE::Sha256Loaded) > 0;
    }

    bool isGeometryLoaded() const override
    {
        return getFlag(Photo::FlagsE::GeometryLoaded) > 0;
    }

    bool isExifDataLoaded() const override
    {
        return getFlag(Photo::FlagsE::ExifLoaded) > 0;
    }

    void setData(const Photo::Data& data) override
    {
        m_data = data;
    }

    void setSha256(const Photo::Sha256sum& sha) override
    {
        m_data.sha256Sum = sha;
        m_data.flags[Photo::FlagsE::Sha256Loaded] = 1;
    }

    void setTags(const Tag::TagsList& tags) override
    {
        m_data.tags = tags;
    }

    void setTag(const TagTypes& type, const TagValue& value) override
    {
        m_data.tags[type] = value;
    }

    void setGeometry(const QSize& geometry) override
    {
        m_data.geometry = geometry;
        m_data.flags[Photo::FlagsE::GeometryLoaded] = 1;
    }

    void setGroup(const GroupInfo& group) override
    {
        m_data.groupInfo = group;
    }

    void markFlag(Photo::FlagsE flag, int value) override
    {
        m_data.flags[flag] = value;
    }

    int getFlag(Photo::FlagsE flag) const override
    {
        auto it = m_data.flags.find(flag);

        return it == m_data.flags.end()? 0: it->second;
    }

private:
    Photo::Data m_data;
};

#endif
