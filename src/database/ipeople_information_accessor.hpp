
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
            virtual std::map<PersonInfo::Id, Person::Fingerprint> fingerprintsFor(const std::vector<PersonInfo::Id>& id) = 0;
    };
}

#endif
