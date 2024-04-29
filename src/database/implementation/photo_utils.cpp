
#include "photo_utils.hpp"

namespace Photo
{
    const QString& getPath(const Photo::DataDelta& data)
    {
        return data.get<Photo::Field::Path>();
    }
}
