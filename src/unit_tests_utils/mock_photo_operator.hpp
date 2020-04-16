
#include <gmock/gmock.h>

#include <database/iphoto_operator.hpp>
#include <database/filter.hpp>


class PhotoOperatorMock: public Database::IPhotoOperator
{
    public:
        MOCK_METHOD(bool, removePhoto, (const Photo::Id &), (override));
        MOCK_METHOD(bool, removePhotos, (const std::vector<Database::IFilter::Ptr> &), (override));
        MOCK_METHOD(std::vector<Photo::Id>, onPhotos, (const std::vector<Database::IFilter::Ptr> &, const Database::Actions &), (override));
        MOCK_METHOD(std::vector<Photo::Id>, getPhotos, (const std::vector<Database::IFilter::Ptr> &), (override));
};
