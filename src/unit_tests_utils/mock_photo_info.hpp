
#ifndef MOCK_PHOTO_INFO_HPP
#define MOCK_PHOTO_INFO_HPP

#include <gmock/gmock.h>

#include <database/iphoto_info.hpp>


// trick with proxies: http://stackoverflow.com/questions/7616475/can-google-mock-a-method-with-a-smart-pointer-return-type
struct MockPhotoInfo: IPhotoInfo
{
    MOCK_CONST_METHOD0(data, Photo::Data());
    MOCK_CONST_METHOD0(getPath, const QString());
    MOCK_CONST_METHOD0(getTags, const Tag::TagsList());
    MOCK_CONST_METHOD0(getGeometry, const QSize());
    MOCK_CONST_METHOD0(getSha256, const Photo::Sha256sum());
    MOCK_CONST_METHOD0(getID, Photo::Id());

    MOCK_CONST_METHOD0(isFullyInitialized, bool());
    MOCK_CONST_METHOD0(isSha256Loaded, bool());
    MOCK_CONST_METHOD0(isGeometryLoaded, bool());
    MOCK_CONST_METHOD0(isExifDataLoaded, bool());

    MOCK_METHOD1(registerObserver, void(IObserver *));
    MOCK_METHOD1(unregisterObserver, void(IObserver *));

    MOCK_METHOD1(setSha256, void(const Photo::Sha256sum &));
    MOCK_METHOD1(setTags, void(const Tag::TagsList &));
    MOCK_METHOD2(setTag, void(const TagNameInfo &, const TagValue &));
    MOCK_METHOD1(setGeometry, void(const QSize &));
    MOCK_METHOD1(setGroup, void(const Group::Id &));

    MOCK_METHOD2(markFlag, void(Photo::FlagsE, int));
    MOCK_CONST_METHOD1(getFlag, int(Photo::FlagsE));

    MOCK_METHOD0(invalidate, void());
    MOCK_METHOD0(isValid, bool());

    //data
    std::mutex m_tagAccessMutex;
};

#endif
