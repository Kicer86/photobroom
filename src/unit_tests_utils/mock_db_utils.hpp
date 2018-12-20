
#ifndef MOCK_DB_UTILS_HPP
#define MOCK_DB_UTILS_HPP


#include <gmock/gmock.h>

#include <database/idatabase.hpp>


struct MockUtils: Database::IUtils
{
    MOCK_METHOD1(getPhotoFor, IPhotoInfo::Ptr(const Photo::Id &));
    MOCK_METHOD1(insertPhotos, std::vector<Photo::Id>(const std::vector<Photo::DataDelta> &));
};


#endif

