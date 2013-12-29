
#ifndef DATABASE_BACKEND_HPP
#define DATABASE_BACKEND_HPP

#include "idatabase.hpp"

namespace Database
{

    class MySqlBackend: public IBackend
    {
        public:
            MySqlBackend() noexcept(true);
            virtual ~MySqlBackend();

            bool init() noexcept(true);
            virtual bool store(const Entry& );
    };

}

#endif
