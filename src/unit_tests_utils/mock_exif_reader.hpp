
#include <any>
#include <optional>

#include <gmock/gmock.h>

#include <core/iexif_reader.hpp>

#include "general.hpp"

struct MockExifReader: IExifReader
{
    MOCK_METHOD1(hasExif, bool(const QString &));
    MOCK_METHOD1(getTagsFor, Tag::TagsList(const QString &));
    MOCK_METHOD2(get, std::optional<std::any>(const QString &, const TagType &));
};
