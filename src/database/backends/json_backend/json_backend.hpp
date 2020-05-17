
#ifndef JSONBACKEND_HPP
#define JSONBACKEND_HPP

#include "database/ibackend.hpp"
#include "database/iphoto_change_log_operator.hpp"


namespace Database
{
    /**
    * \brief json based backend
    */
    class JsonBackend: public IBackend,
                              IPeopleInformationAccessor,
                              IPhotoChangeLogOperator
    {
        public:
            // IBackend interface
            bool addPhotos(std::vector<Photo::DataDelta>& photos) override;
            bool update(const Photo::DataDelta& delta) override;
            std::vector<TagTypeInfo> listTags() override;
            std::vector<TagValue> listTagValues(const TagTypes &, const std::vector<IFilter::Ptr> &) override;
            Photo::Data getPhoto(const Photo::Id &) override;
            int getPhotosCount(const std::vector<IFilter::Ptr> &) override;
            void set(const Photo::Id& id, const QString& name, int value) override;
            std::optional<int> get(const Photo::Id& id, const QString& name) override;
            std::vector<Photo::Id> markStagedAsReviewed() override;
            BackendStatus init(const ProjectInfo &) override;
            void closeConnections() override;
            IGroupOperator& groupOperator() override;
            IPhotoOperator& photoOperator() override;
            IPhotoChangeLogOperator& photoChangeLogOperator() override;
            IPeopleInformationAccessor& peopleInformationAccessor() override;

        private:
            // IPeopleInformationAccessor interface
            std::vector<PersonName> listPeople() override;
            std::vector<PersonInfo> listPeople(const Photo::Id &) override;
            PersonName person(const Person::Id &) override;
            std::vector<PersonFingerprint> fingerprintsFor(const Person::Id &) override;
            std::map<PersonInfo::Id, PersonFingerprint> fingerprintsFor(const std::vector<PersonInfo::Id>& id) override;
            Person::Id store(const PersonName& pn) override;
            PersonInfo::Id store(const PersonInfo& pi) override;
            PersonFingerprint::Id store(const PersonFingerprint &) override;

            // IPhotoChangeLogOperator interface
            void storeDifference(const Photo::Data &, const Photo::DataDelta &) override;
            void groupCreated(const Group::Id &, const Group::Type &, const Photo::Id& representative) override;
            void groupDeleted(const Group::Id &, const Photo::Id &representative, const std::vector<Photo::Id>& members) override;
            QStringList dumpChangeLog() override;

            typedef std::map<QString, int> Flags;

            static Photo::Id getIdFor(const Photo::Data& d)
            {
                return d.id;
            }

            static Person::Id getIdFor(const PersonName& pn)
            {
                return pn.id();
            }

            template<typename T, typename IdT>
            struct IdComparer
            {
                bool operator()(const T& lhs, const T& rhs) const
                {
                    return getIdFor(lhs) < getIdFor(rhs);
                }

                bool operator()(const IdT& lhs, const T& rhs) const
                {
                    return lhs < getIdFor(rhs);
                }

                bool operator()(const T& lhs, const IdT& rhs) const
                {
                    return getIdFor(lhs) < rhs;
                }

                using is_transparent = void;
            };

            std::map<Photo::Id, Flags> m_flags;
            std::map<Photo::Id, std::vector<PersonInfo>> m_people;
            std::set<Photo::Data, IdComparer<Photo::Data, Photo::Id>> m_photos;
            std::set<PersonName, IdComparer<PersonName, Person::Id>> m_peopleNames;

            int m_nextId = 0;
            int m_nextPerson = 0;
    };
}

#endif // JSONBACKEND_HPP
