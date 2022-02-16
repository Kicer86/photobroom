
#ifndef QMODEL_UTILS_HPP_INCLUDED
#define QMODEL_UTILS_HPP_INCLUDED

#include <type_traits>
#include <QByteArray>
#include <QHash>
#define MAGIC_ENUM_RANGE_MIN 0
#define MAGIC_ENUM_RANGE_MAX 512
#include <magic_enum.hpp>


template<typename T, int i, int Count> requires std::is_enum_v<T> && (i < Count)
constexpr void _parseRoles(std::array<std::pair<int, QByteArray>, Count>& output)
{
    constexpr const T value = magic_enum::enum_value<T>(i);
    constexpr const std::string_view fullName = magic_enum::enum_name(value);
    static_assert(fullName.size() > 4 && fullName.substr(fullName.size() - 4) == "Role", "enum entry needs to end with 'Role'");

    constexpr std::string_view name = fullName.substr(0, fullName.size() - 4);

    QByteArray lowerCaseName(name.data(), name.size());
    lowerCaseName[0] = static_cast<char>(std::tolower(lowerCaseName[0]));

    output[i] = std::make_pair(value, lowerCaseName);

    if constexpr (i + 1 < Count)
        _parseRoles<T, i + 1, Count>(output);
}

template<typename T> requires std::is_enum_v<T>
constexpr auto parseRoles()
{
    constexpr auto count = magic_enum::enum_count<T>();
    static_assert(count > 0, "Enum is empty");

    std::array<std::pair<int, QByteArray>, count> output;
    _parseRoles<T, 0, count>(output);

    return output;
}

#endif // QMODEL_UTILS_HPP_INCLUDED
