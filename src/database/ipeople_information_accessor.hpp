
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

            /**
            * \brief get person details
            * \arg id person id
            * \return PersonName struct
            */
            virtual PersonName               person(const Person::Id &) = 0;

            virtual std::vector<PersonFingerprint> fingerprintsFor(const Person::Id &) = 0;
            virtual std::map<PersonInfo::Id, PersonFingerprint> fingerprintsFor(const std::vector<PersonInfo::Id>& id) = 0;

            /**
            * \brief Store or update person
            * \arg pn Details about person name to be stored.
            * \return id of created or updated person.
            *
            * If \a pn has valid id then person name will be updated in database   \n
            * If id is not valid and \a pn holds name already existing in database \n
            * then method will return id of that person.                           \n
            * If id is not valid and name was not found in database then           \n
            * new person will be added to database and its id will be returned.    \n
            */
            virtual Person::Id               store(const PersonName& pn) = 0;

            /**
            * \brief Store or update person details
            * \arg pi Details about person. It needs to refer to a valid photo id.  \n
            *         Also at least one of \a rect, \a person or \a id need to be valid
            *
            * If \a pi has valid id and rect is invalid and person id is not valid, \n
            * then information about person is removed.                             \n
            * if \a pi has invalid id then database will be searched for exisiting  \n
            * rect or person matching information in \a pi. If found, id will be    \n
            * updated and any not stored detail (rect or person) will be updated.
            */
            virtual PersonInfo::Id           store(const PersonInfo& pi) = 0;

            virtual PersonFingerprint::Id    store(const PersonFingerprint &) = 0;
    };
}

#endif
