
#ifndef APEOPLE_INFORMATION_ACCESSOR_HPP
#define APEOPLE_INFORMATION_ACCESSOR_HPP

#include "ipeople_information_accessor.hpp"
#include "database_export.h"


namespace Database
{
    class DATABASE_EXPORT APeopleInformationAccessor: public IPeopleInformationAccessor
    {
        public:
            PersonInfo::Id store(const PersonInfo& pi) override;

        private:
            virtual void dropPersonInfo(const PersonInfo::Id &) = 0;
            virtual PersonInfo::Id storePerson(const PersonInfo &) = 0;
    };
}

#endif
