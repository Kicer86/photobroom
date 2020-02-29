
#ifndef IPEOPLE_INFORMATION_ACCESOR_HPP
#define IPEOPLE_INFORMATION_ACCESOR_HPP

#include "person_data.hpp"


namespace Database
{
    class IPeopleInformationAccessor
    {
        public:
            virtual ~IPeopleInformationAccessor() = default;

            virtual std::vector<Person::Fingerprint> fingerprintsFor(const Person::Id &) = 0;
            virtual std::vector<Person::Fingerprint> fingerprintsFor(const PersonInfo::Id &) = 0;
    };
}

#endif
