
#ifndef QMODEL_UTILS_HPP_INCLUDED
#define QMODEL_UTILS_HPP_INCLUDED

#include <type_traits>
#include <QByteArray>
#include <QHash>
#define MAGIC_ENUM_RANGE_MIN 0
#define MAGIC_ENUM_RANGE_MAX 512
#include <magic_enum.hpp>


template<typename T> requires std::is_enum_v<T>
QHash<int, QByteArray> parseRoles()
{
    QHash<int, QByteArray> roles;
    constexpr auto entries = magic_enum::enum_entries<T>();

    for(auto& entry: entries)
    {
        const int& value = entry.first;
        const std::string_view& fullName = entry.second;
        assert(fullName.size() > 4);
        assert(fullName.substr(fullName.size() - 4) == "Role");

        const std::string_view name = fullName.substr(0, fullName.size() - 4);

        std::string lowerCaseName(name.cbegin(), name.cend());
        lowerCaseName[0] = static_cast<char>(std::tolower(lowerCaseName[0]));

        roles.insert(value, QByteArray::fromStdString(lowerCaseName));
    }

    return roles;
}

#endif // QMODEL_UTILS_HPP_INCLUDED
