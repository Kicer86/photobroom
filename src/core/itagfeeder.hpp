
#ifndef CORE_TAG_FEEDER_HPP
#define CORE_TAG_FEEDER_HPP

#include <memory>
#include <string>

#include "tag.hpp"

class QString;

class TagDataBase;

struct ITagFeeder
{
    virtual ~ITagFeeder();

    virtual Tag::TagsList getTagsFor(const QString& path) = 0;
    //virtual void update(TagDataBase *, const QString& path) = 0;
};

struct ITagFeederFactory
{
    virtual ~ITagFeederFactory();

    virtual std::shared_ptr<ITagFeeder> get() = 0;
};

#endif
