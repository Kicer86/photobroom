
#ifndef DATABASE_BACKEND_HPP
#define DATABASE_BACKEND_HPP

#include "idatabase.hpp"

namespace Database
{

    class DefaultBackend: public IBackend
    {
        public:
            DefaultBackend();
            virtual ~DefaultBackend();

            virtual bool store(const Entry& );
    };

}

#endif
