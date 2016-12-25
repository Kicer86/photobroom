
#ifndef CORE_TAG_FEEDER_HPP
#define CORE_TAG_FEEDER_HPP

#include <memory>
#include <string>

#include <boost/any.hpp>

#include "tag.hpp"

#include "core_export.h"

class QString;

class TagDataBase;


struct CORE_EXPORT IExifReader
{
    enum class ExtraData
    {
        SequenceNumber,            // int
    };

    virtual ~IExifReader() = default;

    virtual Tag::TagsList getTagsFor(const QString& path) = 0;            // returns default set of tags
    virtual boost::any get(const QString& path, const ExtraData &) = 0;   // access to optional data
};


struct CORE_EXPORT IExifReaderFactory
{
    virtual ~IExifReaderFactory() = default;

    virtual std::shared_ptr<IExifReader> get() = 0;
};

#endif
