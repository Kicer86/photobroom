
#ifndef TAG_FEEDER_HPP
#define TAG_FEEDER_HPP

#include <memory>

struct ITagData;

class TagFeeder
{
    public:
        TagFeeder() = delete;

        static std::unique_ptr<ITagData> getTagsFor(const std::string& path);
        static void update(ITagData *, const std::string& path);
};

#endif
