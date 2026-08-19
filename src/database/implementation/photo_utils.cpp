
#include "photo_utils.hpp"

namespace Photo
{
    const Filesystem::Location& getPath(const Photo::DataDelta& data)
    {
        return data.get<Photo::Field::Path>();
    }
}
