
#ifndef EXPLICIT_PHOTO_DELTA_HPP_INCLUDED
#define EXPLICIT_PHOTO_DELTA_HPP_INCLUDED

#include <stdexcept>

#include <core/generic_concepts.hpp>

#include "photo_data.hpp"


namespace Photo
{

    template<Photo::Field... dataFields>
    class ExplicitDelta
    {
    public:
        explicit ExplicitDelta(const DataDelta& delta)
            : m_data(delta)
        {
            validateData();
        }

        template<Field field>
        const typename DeltaTypes<field>::Storage& get() const
        {
            auto is = [](Field f) { return f == field; };

            static_assert( (... || is(dataFields)), "ExplicitDelta has no required Photo::Field" );

            return m_data.get<field>();
        }

    private:
        const DataDelta m_data;

        void validateData()
        {
            auto isValid = [this](Photo::Field field)
            {
                if (m_data.has(field) == false)
                    //throw std::invalid_argument(std::format("Photo::Field: {} was expected to be present in DataDelta", magic_enum::enum_name(field))); TODO: enable, when <format> is available (possibly in gcc 13)
                    throw std::invalid_argument("Missing expected field");
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

}


#endif
