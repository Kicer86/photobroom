
#include "backend.hpp"

#include "entry.hpp"

namespace Database
{

    DefaultBackend::DefaultBackend()
    {
    }


    DefaultBackend::~DefaultBackend()
    {
    }


    bool DefaultBackend::store(const Entry &entry)
    {
        (void) entry;
        return true;
    }

}
