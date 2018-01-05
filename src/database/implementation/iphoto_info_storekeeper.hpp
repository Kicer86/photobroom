
#ifndef IPHOTO_INFO_STOREKEEPER_HPP
#define IPHOTO_INFO_STOREKEEPER_HPP

#include "photo_data.hpp"

struct IPhotoInfoStorekeeper
{
    virtual ~IPhotoInfoStorekeeper() = default;

    virtual void update(const Photo::DataDelta &) = 0;
};

#endif
