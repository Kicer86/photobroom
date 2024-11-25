
#ifndef EXPLICIT_PHOTO_DELTA_HPP_INCLUDED
#define EXPLICIT_PHOTO_DELTA_HPP_INCLUDED

#include <stdexcept>

#include <magic_enum/magic_enum.hpp>

#include <core/generic_concepts.hpp>
#include "photo_data.hpp"


namespace Photo
{

    template<Photo::Field... dataFields>
    class ExplicitDelta
    {
    public:
        ExplicitDelta()
            : m_data()
        {
            fill<dataFields...>();
        }

        explicit ExplicitDelta(const Photo::Id& id)
            : m_data(id)
        {
            fill<dataFields...>();
        }

        explicit ExplicitDelta(const DataDelta& delta)
        {
            validate(delta);
            fill<dataFields...>();
            m_data |= delta;
        }

        ExplicitDelta(const ExplicitDelta& other) noexcept
        {
            m_data = other.m_data;
        }

        template<Photo::Field... otherFields>
        ExplicitDelta(const ExplicitDelta<otherFields...>& other) noexcept
        {
            static_assert( (... && ExplicitDelta<otherFields...>::template has<dataFields>()), "Other object needs to be superset of this");

            fill<dataFields...>();
            m_data |= other.m_data;
        }

        explicit ExplicitDelta(ExplicitDelta&& other) noexcept
        {
            m_data = std::move(other.m_data);
        }

        ExplicitDelta& operator=(const ExplicitDelta& other)
        {
            m_data = other.m_data;

            return *this;
        }

        template<Photo::Field... otherFields>
        ExplicitDelta& operator|=(const ExplicitDelta<otherFields...>& other)
        {
            static_assert( (... && has<otherFields>()), "Other object needs to be subset of this");

            m_data |= other.m_data;

            return *this;
        }

        ExplicitDelta& operator|=(const DataDelta& other)
        {
            validate(other);

            fill<dataFields...>();
            m_data |= other;

            return *this;
        }

        bool operator==(const ExplicitDelta<dataFields...> &) const = default;

        operator const DataDelta& () const
        {
            return m_data;
        }

        const Id& getId() const
        {
            return m_data.getId();
        }

        template<Field field>
        const typename DeltaTypes<field>::Storage& get() const
        {
            static_assert(has<field>(), "ExplicitDelta has no required Photo::Field");

            return m_data.get<field>();
        }

        template<Field field>
        typename DeltaTypes<field>::Storage& get()
        {
            static_assert(has<field>(), "ExplicitDelta has no required Photo::Field");

            return m_data.get<field>();
        }

        template<Field field>
        void insert(const typename DeltaTypes<field>::Storage& d)
        {
            static_assert(has<field>(), "ExplicitDelta has no required Photo::Field");

            return m_data.insert<field>(d);
        }

    private:
        template<typename Photo::Field...>
        friend class ExplicitDelta;

        DataDelta m_data;

        void validate(const Photo::DataDelta& other) const
        {
            for(const Photo::Field field : magic_enum::enum_values<Photo::Field>())
                if (other.has(field) && has(field) == false)
                    throw std::invalid_argument(std::string("Photo::Field: ") + magic_enum::enum_name(field).data() + " from DataDelta is not part of this ExplicitDelta.");
        }

        template<Field field, Field... fields>
        void fill()
        {
            m_data.insert<field>({});

            if constexpr (sizeof...(fields) > 0)
                fill<fields...>();
        }

        template<Field field>
        constexpr static bool has()
        {
            auto is = [](Field f) { return f == field; };

            return (... || is(dataFields));
        }

        static bool has(const Field& field)
        {
            auto is = [field](Field f) { return f == field; };

            return (... || is(dataFields));
        }
    };

    // convert Container<Photo::DataDelta> to std::vector<T> where T is supposed to be ExplicitDelta
    template<typename T, Container C>
    std::vector<T> EDV(const C& c)
    requires std::is_same_v<typename C::value_type, DataDelta>
    {
        return std::vector<T>(c.begin(), c.end());
    }

    // convert vector of ExplicitDelta to std::vector ExplicitDelta
    template<typename T, Photo::Field... fields>
    std::vector<T> EDV(const std::vector<ExplicitDelta<fields...>>& c)
    {
        return std::vector<T>(c.begin(), c.end());
    }

    // based on: https://stackoverflow.com/questions/60434033/how-do-i-expand-a-compile-time-stdarray-into-a-parameter-pack
    namespace details
    {
        template<auto arr, typename IS = decltype(std::make_index_sequence<arr.size()>())> struct Generator;

        template<auto arr, std::size_t... I>
        struct Generator<arr, std::index_sequence<I...>> {
            using type = ExplicitDelta<arr[I]...>;
        };

        template<auto arr>
        using Generator_t = typename Generator<arr>::type;
    }

    using FullDelta = details::Generator_t<magic_enum::enum_values<Photo::Field>()>;
}


#endif
