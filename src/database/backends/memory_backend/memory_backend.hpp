
#ifndef MEMORYBACKEND_HPP
#define MEMORYBACKEND_HPP

#include "database/aphoto_change_log_operator.hpp"
#include "database/apeople_information_accessor.hpp"
#include "database/ibackend.hpp"
#include "database/igroup_operator.hpp"
#include "database/iphoto_operator.hpp"

#include "database_memory_backend_export.h"


namespace Database
{
    /**
    * \brief memory based backend
    */
    class DATABASE_MEMORY_BACKEND_EXPORT MemoryBackend:
        public IBackend,
               APeopleInformationAccessor,
               APhotoChangeLogOperator,
               IGroupOperator,
               IPhotoOperator
    {
        public:
            // IBackend interface
            bool addPhotos(std::vector<Photo::DataDelta>& photos) override;
            bool update(const std::vector<Photo::DataDelta> &) override;
            std::vector<TagValue> listTagValues(const TagTypes &, const Filter &) override;
            Photo::Data getPhoto(const Photo::Id &) override;
            Photo::DataDelta getPhotoDelta(const Photo::Id &, const std::set<Photo::Field> &) override;
            int getPhotosCount(const Filter &) override;
            void set(const Photo::Id& id, const QString& name, int value) override;
            std::optional<int> get(const Photo::Id& id, const QString& name) override;
            void setThumbnail(const Photo::Id &, const QByteArray &) override;
            QByteArray getThumbnail(const Photo::Id &) override;
            std::vector<Photo::Id> markStagedAsReviewed() override;
            BackendStatus init(const ProjectInfo &) override;
            void closeConnections() override;
            IGroupOperator& groupOperator() override;
            IPhotoOperator& photoOperator() override;
            IPhotoChangeLogOperator& photoChangeLogOperator() override;
            IPeopleInformationAccessor& peopleInformationAccessor() override;

        private:
            // APeopleInformationAccessor interface
            std::vector<PersonName> listPeople() override;
            std::vector<PersonInfo> listPeople(const Photo::Id &) override;
            PersonName person(const Person::Id &) override;
            std::vector<PersonFingerprint> fingerprintsFor(const Person::Id &) override;
            std::map<PersonInfo::Id, PersonFingerprint> fingerprintsFor(const std::vector<PersonInfo::Id>& id) override;
            Person::Id store(const PersonName& pn) override;
            PersonFingerprint::Id store(const PersonFingerprint &) override;
            void dropPersonInfo(const PersonInfo::Id &) override;
            PersonInfo::Id storePerson(const PersonInfo &) override;

            // APhotoChangeLogOperator interface
            void append(const Photo::Id &, Operation, Field, const QString& data) override;
            QStringList dumpChangeLog() override;

            // IGroupOperator interface
            Group::Id addGroup(const Photo::Id& representative_photo, Group::Type) override;
            Photo::Id removeGroup(const Group::Id &) override;
            Group::Type type(const Group::Id &) const override;
            std::vector<Photo::Id> membersOf(const Group::Id &) const override;

            // IPhotoOperator interface
            bool removePhoto(const Photo::Id &) override;
            bool removePhotos(const Filter &) override;
            std::vector<Photo::Id> onPhotos(const Filter &, const Action &) override;
            std::vector<Photo::Id> getPhotos(const Filter &) override;

            //
            typedef std::map<QString, int> Flags;
            typedef std::pair<Photo::Id, Group::Type> GroupData;
            typedef std::tuple<Photo::Id, Operation, Field, QString> LogEntry;

            static Photo::Id getIdFor(const Photo::Data& d);
            static Person::Id getIdFor(const PersonName& pn);
            static PersonInfo::Id getIdFor(const PersonInfo& pn);

            void onPhotos(std::vector<Photo::Data> &, const Action &) const;

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
            std::map<Group::Id, GroupData> m_groups;
            std::set<Photo::Data, IdComparer<Photo::Data, Photo::Id>> m_photos;
            std::set<PersonName, IdComparer<PersonName, Person::Id>> m_peopleNames;
            std::set<PersonInfo, IdComparer<PersonInfo, PersonInfo::Id>> m_peopleInfo;
            std::vector<LogEntry> m_logEntries;
            std::map<Photo::Id, QByteArray> m_thumbnails;

            int m_nextPhotoId = 0;
            int m_nextPersonName = 0;
            int m_nextGroup = 0;
            int m_nextPersonInfo = 0;
    };
}

#endif // MEMORYBACKEND_HPP
