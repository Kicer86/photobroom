
#ifndef IPEOPLE_INFORMATION_ACCESOR_HPP
#define IPEOPLE_INFORMATION_ACCESOR_HPP

#include "person_data.hpp"

class IPeopleInformationAccessor
{
    public:
        virtual ~IPeopleInformationAccessor() = default;

        /// list all people names
        virtual std::vector<PersonName>  listPeople() = 0;

        /**
         * \brief get person details
         * \arg id person id
         * \return PersonName struct
         */
        virtual PersonName               person(const Person::Id &) = 0;
};

#endif
