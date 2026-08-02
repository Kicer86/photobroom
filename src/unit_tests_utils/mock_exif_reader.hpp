#ifndef MOCK_EXIF_READER_HPP_INCLUDED
#define MOCK_EXIF_READER_HPP_INCLUDED


#include <any>
#include <optional>

#include <gmock/gmock.h>

#include <core/filesystem.hpp>
#include <core/iexif_reader.hpp>


struct MockExifReader: IExifReader
{
    MOCK_METHOD(bool, hasExif, (const Filesystem::Location &), (override));
    MOCK_METHOD(Tag::TagsList, getTagsFor, (const Filesystem::Location &), (override));
    MOCK_METHOD(std::optional<std::any>, get, (const Filesystem::Location &, const TagType &), (override));
};

#endif
