
#ifndef DATABASE_CLIENT_HPP_INCLUDED
#define DATABASE_CLIENT_HPP_INCLUDED

#include "database_export.h"

#include "idatabase.hpp"


namespace Database
{
    class DATABASE_EXPORT Client
    {
    public:
        explicit Client(IDatabase *);
    };
}

#endif
