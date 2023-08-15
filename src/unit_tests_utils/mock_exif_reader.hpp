#ifndef MOCK_EXIF_READER_HPP_INCLUDED
#define MOCK_EXIF_READER_HPP_INCLUDED


#include <any>
#include <optional>

#include <gmock/gmock.h>

#include <core/iexif_reader.hpp>

#include "general.hpp"

struct MockExifReader: IExifReader
{
    MOCK_METHOD(bool, hasExif, (const QString &), (override));
    MOCK_METHOD(Tag::TagsList, getTagsFor, (const QString &), (override));
    MOCK_METHOD(std::optional<std::any>, get, (const QString &, const TagType &), (override));
};

#endif
