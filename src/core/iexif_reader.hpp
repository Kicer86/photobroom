
#ifndef CORE_TAG_FEEDER_HPP
#define CORE_TAG_FEEDER_HPP

#include <any>
#include <memory>
#include <optional>
#include <string>

#include "tag.hpp"

#include "core_export.h"

class QString;

class TagDataBase;


struct CORE_EXPORT IExifReader
{
    enum class TagType
    {
        DateTimeOriginal,           // string
        Orientation,                // int
        SequenceNumber,             // int
        PixelXDimension,            // long
        PixelYDimension,            // long
        Exposure,                   // float
        Projection,                 // string
        ShutterSpeed,               // float (seconds)
    };

    virtual ~IExifReader() = default;

    virtual bool hasExif(const QString& path) = 0;

    virtual Tag::TagsList getTagsFor(const QString& path) = 0;                       // returns default set of tags
    virtual std::optional<std::any> get(const QString& path, const TagType &) = 0;   // access to optional data
};


struct CORE_EXPORT IExifReaderFactory
{
    virtual ~IExifReaderFactory() = default;

    virtual IExifReader& get() = 0;
};

#endif
