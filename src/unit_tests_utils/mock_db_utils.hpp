
#ifndef MOCK_DB_UTILS_HPP
#define MOCK_DB_UTILS_HPP


#include <gmock/gmock.h>

#include <database/idatabase.hpp>


struct MockUtils: Database::IUtils
{
    MOCK_METHOD(IPhotoInfo::Ptr, getPhotoFor, (const Photo::Id &), (override));
};


#endif
