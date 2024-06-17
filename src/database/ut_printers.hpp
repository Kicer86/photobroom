
#pragma once

#include <iostream>

#include "photo_data.hpp"

namespace Photo
{
    inline void PrintTo(const DataDelta& delta, ::std::ostream* os)
    {
        *os << "{";
        *os << " id: " << (delta.getId().valid()? delta.getId().value(): -1);
        *os << "}";
    }
}
