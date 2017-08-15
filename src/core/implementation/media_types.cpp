
#include "media_types.hpp"

namespace
{
    template<typename T>
    std::vector<T> merge(const std::vector<T>& v1, const std::vector<T>& v2)
    {
        std::vector<T> result;
        result.reserve(v1.size() + v2. size());

        result.insert(result.end(), v1.begin(), v1.end());
        result.insert(result.end(), v2.begin(), v2.end());

        return result;
    }
}


namespace MediaTypes
{
    const std::vector<const char *> images_extensions = { "jpe?g", "png", "gif", "tif?f", "bmp" };
    const std::vector<const char *> movies_extensions = { "avi", "mkv", "mpe?g", "mp4" };

    const std::vector<const char *> all_extensions = merge(images_extensions, movies_extensions);
}
