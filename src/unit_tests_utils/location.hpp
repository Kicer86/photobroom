#ifndef UNIT_TESTS_UTILS_LOCATION_HPP
#define UNIT_TESTS_UTILS_LOCATION_HPP

#include <string_view>

#include <QStringView>

#include "core/filesystem.hpp"


namespace UnitTests
{
    [[nodiscard]] inline Filesystem::Location makeLocation(const QStringView& path)
    {
        return Filesystem::Location(path);
    }

    [[nodiscard]] inline Filesystem::Location makeLocation(const std::string_view& path)
    {
        return Filesystem::Location(path);
    }
}

#endif
