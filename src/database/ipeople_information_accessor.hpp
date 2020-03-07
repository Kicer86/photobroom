
#ifndef IPEOPLE_INFORMATION_ACCESOR_HPP
#define IPEOPLE_INFORMATION_ACCESOR_HPP

#include "person_data.hpp"


namespace Database
{
    class IPeopleInformationAccessor
    {
        public:
            virtual ~IPeopleInformationAccessor() = default;

            /// list all people names
            virtual std::vector<PersonName>  listPeople() = 0;

            /// list people on photo
            virtual std::vector<PersonInfo>  listPeople(const Photo::Id &) = 0;

            virtual std::vector<PersonFingerprint> fingerprintsFor(const Person::Id &) = 0;
            virtual std::map<PersonInfo::Id, PersonFingerprint> fingerprintsFor(const std::vector<PersonInfo::Id>& id) = 0;
            virtual PersonFingerprint::Id store(const PersonFingerprint &) = 0;
    };
}

#endif
