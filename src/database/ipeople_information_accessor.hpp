
#ifndef IPEOPLE_INFORMATION_ACCESOR_HPP
#define IPEOPLE_INFORMATION_ACCESOR_HPP

#include "person_data.hpp"


namespace Database
{
    class IPeopleInformationAccessor
    {
        public:
            virtual ~IPeopleInformationAccessor() = default;

            virtual std::vector<PersonFingerprint> fingerprintsFor(const Person::Id &) = 0;
            virtual std::map<PersonInfo::Id, PersonFingerprint> fingerprintsFor(const std::vector<PersonInfo::Id>& id) = 0;
            virtual void store(const PersonFingerprint &) = 0;
    };
}

#endif
