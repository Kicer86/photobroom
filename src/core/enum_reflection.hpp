#ifndef ENUM_REFLECTION_HPP_INCLUDED
#define ENUM_REFLECTION_HPP_INCLUDED

#include <array>
#include <string_view>
#include <type_traits>

#ifndef PHOTO_BROOM_HAS_CPP26_REFLECTION
#define PHOTO_BROOM_HAS_CPP26_REFLECTION 0
#endif

#if PHOTO_BROOM_HAS_CPP26_REFLECTION

#include <meta>

namespace reflection
{
    template<typename E>
    requires std::is_enum_v<E>
    inline constexpr auto enum_values = []() consteval
    {
        static constexpr auto enumerators = std::define_static_array(std::meta::enumerators_of(^^E));
        std::array<E, enumerators.size()> values{};
        std::size_t index = 0;

        template for (constexpr auto enumerator : enumerators)
            values[index++] = [:enumerator:];

        return values;
    }();

    template<typename E>
    requires std::is_enum_v<E>
    inline constexpr auto enum_names = []() consteval
    {
        static constexpr auto enumerators = std::define_static_array(std::meta::enumerators_of(^^E));
        std::array<std::string_view, enumerators.size()> names{};
        std::size_t index = 0;

        template for (constexpr auto enumerator : enumerators)
            names[index++] = std::meta::identifier_of(enumerator);

        return names;
    }();

    template<typename E>
    requires std::is_enum_v<E>
    constexpr std::string_view enum_name(E value)
    {
        for (std::size_t i = 0; i < enum_values<E>.size(); ++i)
            if (enum_values<E>[i] == value)
                return enum_names<E>[i];

        return {};
    }
}

#else

#include <rfl/enums.hpp>

namespace reflection
{
    template<typename E>
    requires std::is_enum_v<E>
    inline constexpr auto enum_values = []
    {
        constexpr auto enumerators = rfl::get_enumerator_array<E>();
        std::array<E, enumerators.size()> values{};

        for (std::size_t i = 0; i < values.size(); ++i)
            values[i] = enumerators[i].second;

        return values;
    }();

    template<typename E>
    requires std::is_enum_v<E>
    inline constexpr auto enum_names = []
    {
        constexpr auto enumerators = rfl::get_enumerator_array<E>();
        std::array<std::string_view, enumerators.size()> names{};

        for (std::size_t i = 0; i < names.size(); ++i)
            names[i] = enumerators[i].first;

        return names;
    }();

    template<typename E>
    requires std::is_enum_v<E>
    constexpr std::string_view enum_name(E value)
    {
        for (std::size_t i = 0; i < enum_values<E>.size(); ++i)
            if (enum_values<E>[i] == value)
                return enum_names<E>[i];

        return {};
    }
}

#endif

#endif
