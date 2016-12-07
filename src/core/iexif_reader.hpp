
#ifndef CORE_TAG_FEEDER_HPP
#define CORE_TAG_FEEDER_HPP

#include <memory>
#include <string>

#include "tag.hpp"

#include "core_export.h"

class QString;

class TagDataBase;

struct CORE_EXPORT IExifReader
{
    virtual ~IExifReader() = default;

    virtual Tag::TagsList getTagsFor(const QString& path) = 0;
};


struct CORE_EXPORT IExifReaderFactory
{
    virtual ~IExifReaderFactory() = default;

    virtual std::shared_ptr<IExifReader> get() = 0;
};

#endif
