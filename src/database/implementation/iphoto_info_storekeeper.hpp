
#ifndef IPHOTO_INFO_STOREKEEPER_HPP
#define IPHOTO_INFO_STOREKEEPER_HPP

#include "photo_data.hpp"

struct IPhotoInfoStorekeeper
{
    virtual ~IPhotoInfoStorekeeper() = default;

    virtual void storeTags(const Photo::Id &, const Tag::TagsList &) = 0;
};

#endif
