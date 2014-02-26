
#ifndef TAG_FEEDER_HPP
#define TAG_FEEDER_HPP

#include <memory>
#include <string>

#include "core_export.h"

struct ITagData;

class CORE_EXPORT TagFeeder
{
    public:
        TagFeeder() = delete;

        static std::unique_ptr<ITagData> getTagsFor(const std::string& path);
        static void update(ITagData *, const std::string& path);
};

#endif
