
#ifndef MOCK_PHOTO_INFO_HPP
#define MOCK_PHOTO_INFO_HPP

#include <gmock/gmock.h>

#include <database/iphoto_info.hpp>

struct MockPhotoInfo: IPhotoInfo
{
    MOCK_CONST_METHOD0(getPath, const QString&());
    MOCK_CONST_METHOD0(getTags, const Tag::TagsList&());
    MOCK_CONST_METHOD0(getThumbnail, const QPixmap&());
    MOCK_CONST_METHOD0(getHash, const Hash&());
    MOCK_CONST_METHOD0(getID, Id());

    MOCK_CONST_METHOD0(isFullyInitialized, bool());
    MOCK_CONST_METHOD0(isHashLoaded, bool());
    MOCK_CONST_METHOD0(isThumbnailLoaded, bool());
    MOCK_CONST_METHOD0(isExifDataLoaded, bool());

    MOCK_METHOD1(registerObserver, void(IObserver *));
    MOCK_METHOD1(unregisterObserver, void(IObserver *));

    MOCK_METHOD1(initHash, void(const Hash &));
    MOCK_METHOD1(initThumbnail, void(const QPixmap &));
    MOCK_METHOD1(initID, void(const Id &));

    ol::ThreadSafeResource<Tag::TagsList>::Accessor accessTags() override
    {
        return ol::ThreadSafeResource<Tag::TagsList>::Accessor(m_tagAccessMutex, accessTagsProxy());
    }

    MOCK_METHOD1(setTags, void(const Tag::TagsList &));

    MOCK_METHOD1(markStagingArea, void(bool));
    MOCK_METHOD1(markExifDataLoaded, void(bool));
    MOCK_CONST_METHOD0(getFlags, Flags());

    //proxies
    MOCK_METHOD0(accessTagsProxy, Tag::TagsList*());

    //data
    std::mutex m_tagAccessMutex;
};

#endif
