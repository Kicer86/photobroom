
#ifndef CORE_TAG_FEEDER_HPP
#define CORE_TAG_FEEDER_HPP

#include <memory>
#include <string>

#include "core_export.h"

class QString;

struct ITagData;

struct ITagFeeder
{
    virtual ~ITagFeeder() {}

    virtual std::unique_ptr<ITagData> getTagsFor(const QString& path) = 0;
    virtual void update(ITagData *, const QString& path) = 0;
};


struct CORE_EXPORT TagFeederFactory
{
    TagFeederFactory() = delete;

    static std::shared_ptr<ITagFeeder> get();
};

#endif
