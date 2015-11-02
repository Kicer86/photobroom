
#include "photo_types.hpp"

namespace Photo
{

    Id::Id(): m_value(-1), m_valid(false)
    {

    }


    Id::Id(Id::type v): m_value(v), m_valid(true)
    {

    }


    bool Id::operator!() const
    {
        return !m_valid;
    }


    Id::operator Id::type() const
    {
        return m_value;
    }


    bool Id::valid() const
    {
        return m_valid;
    }


    Id::type Id::value() const
    {
        return m_value;
    }
    
}
