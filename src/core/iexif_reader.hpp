
#ifndef CORE_TAG_FEEDER_HPP
#define CORE_TAG_FEEDER_HPP

#include <any>
#include <memory>
#include <optional>
#include <string>

#include "core_export.h"
#include "filesystem.hpp"
#include "tag.hpp"


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
        Xmp_video_DateTimeOriginal, // string
        Xmp_video_Width,            // int
        Xmp_video_Height,           // int
    };

    virtual ~IExifReader() = default;

    virtual bool hasExif(const Filesystem::Location& path) = 0;

    virtual Tag::TagsList getTagsFor(const Filesystem::Location& path) = 0;                       // returns default set of tags
    virtual std::optional<std::any> get(const Filesystem::Location& path, const TagType &) = 0;   // access to optional data
};


struct CORE_EXPORT IExifReaderFactory
{
    virtual ~IExifReaderFactory() = default;

    virtual IExifReader& get() = 0;
};

#endif
