
#include <gmock/gmock.h>

#include <database/iphoto_operator.hpp>
#include <database/filter.hpp>


class PhotoOperatorMock: public Database::IPhotoOperator
{
    public:
        MOCK_METHOD(bool, removePhoto, (const Photo::Id &), (override));
        MOCK_METHOD(bool, removePhotos, (const Database::Filter &), (override));
        MOCK_METHOD(std::vector<Photo::Id>, onPhotos, (const Database::Filter &, const Database::Action &), (override));
        MOCK_METHOD(std::vector<Photo::Id>, getPhotos, (const Database::Filter &), (override));
        MOCK_METHOD(void, setPHash, (const Photo::Id&, const Database::PHash &), (override));
        MOCK_METHOD(std::optional<Database::PHash>, getPHash, (const Photo::Id &), (override));
        MOCK_METHOD(bool, hasPHash, (const Photo::Id &), (override));
};
