
#ifndef EXPLICIT_PHOTO_DELTA_HPP_INCLUDED
#define EXPLICIT_PHOTO_DELTA_HPP_INCLUDED

#include <stdexcept>

#include <core/generic_concepts.hpp>
#include <magic_enum.hpp>

#include "photo_data.hpp"


namespace Photo
{

    template<Photo::Field... dataFields>
    class ExplicitDelta
    {
    public:
        ExplicitDelta(): m_data() {}

        explicit ExplicitDelta(const DataDelta& delta)
            : m_data(delta)
        {
            validateData();
        }

        explicit ExplicitDelta(const ExplicitDelta& other) noexcept
        {
            m_data = other.m_data;
        }

        template<Photo::Field... otherFields>
        explicit ExplicitDelta(const ExplicitDelta<otherFields...>& other) noexcept
        {
            static_assert( (... && ExplicitDelta<otherFields...>::template has<dataFields>()), "Other object needs to be superset of this");

            m_data = other.m_data;
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

    private:
        template<typename Photo::Field... other>
        friend class ExplicitDelta;

        DataDelta m_data;

        template<Field field>
        constexpr static bool has()
        {
            auto is = [](Field f) { return f == field; };

            return (... || is(dataFields));
        }

        void validateData()
        {
            auto isValid = [this](Photo::Field field)
            {
                if (m_data.has(field) == false)
                    throw std::invalid_argument(std::string("Photo::Field: ") + magic_enum::enum_name(field).data() + " was expected to be present in DataDelta");
            };

            (..., isValid(dataFields));
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
}


#endif
