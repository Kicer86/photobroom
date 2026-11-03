#ifndef UNIT_TESTS_UTILS_LOCATION_HPP
#define UNIT_TESTS_UTILS_LOCATION_HPP

#include <string_view>

#include <QStringView>

#include "core/filesystem.hpp"


namespace UnitTests
{
    template<typename T>
    requires std::constructible_from<Filesystem::Location, const T&>
    [[nodiscard]] inline Filesystem::Location makeLocation(const T& path)
    {
        return Filesystem::Location(path);
    }
}

#endif
