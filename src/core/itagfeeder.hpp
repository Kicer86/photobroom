
#ifndef CORE_TAG_FEEDER_HPP
#define CORE_TAG_FEEDER_HPP

#include <memory>
#include <string>

#include "tag.hpp"

#include "core_export.h"

class QString;

class TagDataBase;

struct CORE_EXPORT ITagFeeder
{
    virtual ~ITagFeeder() = default;

    virtual Tag::TagsList getTagsFor(const QString& path) = 0;
};


struct CORE_EXPORT ITagFeederFactory
{
    virtual ~ITagFeederFactory() = default;

    virtual std::shared_ptr<ITagFeeder> get() = 0;
};

#endif
