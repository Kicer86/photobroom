
#ifndef DATABASE_BACKEND_HPP
#define DATABASE_BACKEND_HPP

#include "idatabase.hpp"

namespace Database
{

    class PrimitiveBackend: public IBackend
    {
        public:
            PrimitiveBackend();
            virtual ~PrimitiveBackend();
            
            virtual bool store(const Entry& );
    };
    
}

#endif
