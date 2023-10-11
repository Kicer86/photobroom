
#ifndef FACES_SAVER_HPP_INCLUDED
#define FACES_SAVER_HPP_INCLUDED

#include "people_editor_impl.hpp"


class FacesSaver: public IFacesSaver
{
    public:
        explicit FacesSaver(Database::IDatabase &);
        ~FacesSaver();

        void store(FaceInfo &) override;

    private:
        Database::IDatabase& m_db;
        std::vector<PersonName> m_people;

        void store_person_name(FaceInfo& face);
        void store_fingerprint(FaceInfo& face);
        void store_person_information(const FaceInfo& face);

        std::vector<PersonName> fetchPeople() const;
        PersonName storeNewPerson(const QString& name) const;
};

#endif
