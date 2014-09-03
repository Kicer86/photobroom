
#ifndef CORE_TAG_FEEDER_HPP
#define CORE_TAG_FEEDER_HPP

#include <memory>
#include <string>

class QString;

struct TagDataBase;

struct ITagFeeder
{
    virtual ~ITagFeeder() {}

    virtual std::unique_ptr<TagDataBase> getTagsFor(const QString& path) = 0;
    virtual void update(TagDataBase *, const QString& path) = 0;
};

#endif
