
#ifndef QMODEL_UTILS_HPP_INCLUDED
#define QMODEL_UTILS_HPP_INCLUDED

#include <type_traits>
#include <QByteArray>
#include <QHash>
#include <rfl/enums.hpp>


#define ENUM_ROLES_SETUP(T)                        \
    template <>                                    \
    struct enchantum::enum_traits<T> {             \
    static constexpr int min = 0;                  \
    static constexpr int max = 512;                \
}


#define RETURN_MODEL_ROLES(A, R)                                            \
    auto roles = A::roleNames();                                            \
    const auto extra = parseRoles<R>();                                     \
    const QHash<int, QByteArray> extraRoles(extra.begin(), extra.end());    \
    roles.insert(extraRoles);                                               \
    return roles


template<typename T, int i, int Count> requires std::is_enum_v<T> && (i < Count)
void _parseRoles(std::array<std::pair<int, QByteArray>, Count>& output)
{
    constexpr const auto enumerators = rfl::get_enumerator_array<T>();
    constexpr const T value = enumerators[i].second;
    constexpr const std::string_view fullName = enumerators[i].first;
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
    static_assert(enchantum::enum_traits<T>::min == 0 && enchantum::enum_traits<T>::max == 512,
                  "ENUM_ROLES_SETUP macro needs to be applied for enum with roles."
    );

    constexpr auto count = rfl::get_enumerator_array<T>().size();
    static_assert(count > 0, "Enum is empty");

    std::array<std::pair<int, QByteArray>, count> output;
    _parseRoles<T, 0, count>(output);

    return output;
}

#endif // QMODEL_UTILS_HPP_INCLUDED
