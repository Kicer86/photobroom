
#ifndef MOCK_BACKEND_HPP
#define MOCK_BACKEND_HPP


#include <gmock/gmock.h>

#include <database/filter.hpp>
#include <database/ibackend.hpp>
#include <database/igroup_operator.hpp>
#include <database/iphoto_change_log_operator.hpp>
#include <database/iphoto_operator.hpp>
#include <database/project_info.hpp>


struct MockBackend: public Database::IBackend
{
  MOCK_METHOD1(addPhotos,
      bool(std::vector<Photo::DataDelta> &));
  MOCK_METHOD2(addGroup,
      Group::Id(const Photo::Id &, Group::Type));

  MOCK_METHOD(bool, update, (const std::vector<Photo::DataDelta> &), (override));

  MOCK_METHOD(std::vector<TagValue>, listTagValues, (const TagTypes &, const Database::Filter &), (override));
  MOCK_METHOD0(getAllPhotos,
      std::vector<Photo::Id>());
  MOCK_METHOD1(getPhoto,
      Photo::Data(const Photo::Id &));
  MOCK_METHOD(Photo::DataDelta, getPhotoDelta, (const Photo::Id &, const std::set<Photo::Field> &), (override));
  MOCK_METHOD(int, getPhotosCount, (const Database::Filter &), (override));
  MOCK_METHOD0(listPeople,
      std::vector<PersonName>());
  MOCK_METHOD1(listPeople,
      std::vector<PersonInfo>(const Photo::Id &));
  MOCK_METHOD1(person,
      PersonName(const Person::Id &));
  MOCK_METHOD3(set,
      void(const Photo::Id &, const QString &, int value));
  MOCK_METHOD2(get,
      std::optional<int>(const Photo::Id &, const QString &));
  MOCK_METHOD(void, setThumbnail, (const Photo::Id &, const QByteArray &), (override));
  MOCK_METHOD(QByteArray, getThumbnail, (const Photo::Id &), (override));
  MOCK_METHOD0(markStagedAsReviewed,
      std::vector<Photo::Id>());
  MOCK_METHOD1(init,
      Database::BackendStatus(const Database::ProjectInfo &));
  MOCK_METHOD0(closeConnections,
      void());

  MOCK_METHOD(Database::IGroupOperator&, groupOperator, (), (override));
  MOCK_METHOD(Database::IPhotoOperator&, photoOperator, (), (override));
  MOCK_METHOD(Database::IPhotoChangeLogOperator&, photoChangeLogOperator, (), (override));
  MOCK_METHOD(Database::IPeopleInformationAccessor&, peopleInformationAccessor, (), (override));
};


#endif
