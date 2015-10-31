
#ifndef MOCK_PHOTO_INFO_HPP
#define MOCK_PHOTO_INFO_HPP

#include <gmock/gmock.h>

#include <database/iphoto_info.hpp>


// trick with proxies: http://stackoverflow.com/questions/7616475/can-google-mock-a-method-with-a-smart-pointer-return-type
struct MockPhotoInfo: IPhotoInfo
{
    MOCK_CONST_METHOD0(getPath, const QString());
    MOCK_CONST_METHOD0(getTags, const Tag::TagsList());
    MOCK_CONST_METHOD0(getThumbnail, const QImage());
    MOCK_CONST_METHOD0(getSha256, const Photo::Sha256sum());
    MOCK_CONST_METHOD0(getID, Photo::Id());

    MOCK_CONST_METHOD0(isFullyInitialized, bool());
    MOCK_CONST_METHOD0(isSha256Loaded, bool());
    MOCK_CONST_METHOD0(isThumbnailLoaded, bool());
    MOCK_CONST_METHOD0(isExifDataLoaded, bool());

    MOCK_METHOD1(registerObserver, void(IObserver *));
    MOCK_METHOD1(unregisterObserver, void(IObserver *));

    MOCK_METHOD1(initSha256, void(const Photo::Sha256sum &));
    MOCK_METHOD1(initThumbnail, void(const QImage &));
    MOCK_METHOD1(initID, void(const Photo::Id &));

    MOCK_METHOD1(setTags, void(const Tag::TagsList &));

    MOCK_METHOD1(markStagingArea, void(bool));
    MOCK_METHOD1(markExifDataLoaded, void(bool));
    MOCK_CONST_METHOD0(getFlags, Photo::FlagsE());

    //proxies
    MOCK_METHOD0(accessTagsProxy, Tag::TagsList*());

    //data
    std::mutex m_tagAccessMutex;
};

#endif
