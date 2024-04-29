
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
  MOCK_METHOD(bool, addPhotos, (std::vector<Photo::DataDelta> &), (override));

  MOCK_METHOD(bool, update, (const std::vector<Photo::DataDelta> &), (override));

  MOCK_METHOD(std::vector<TagValue>, listTagValues, (const Tag::Types &, const Database::Filter &), (override));
  MOCK_METHOD(Photo::DataDelta, getPhotoDelta, (const Photo::Id &, const std::set<Photo::Field> &), (override));
  MOCK_METHOD(int, getPhotosCount, (const Database::Filter &), (override));
  MOCK_METHOD(void, set, (const Photo::Id &, const QString &, int value), (override));
  MOCK_METHOD(std::optional<int>, get, (const Photo::Id &, const QString &), (override));
  MOCK_METHOD(void, setBits, (const Photo::Id& id, const QString& name, int bits), (override));
  MOCK_METHOD(void, clearBits, (const Photo::Id& id, const QString& name, int bits), (override));
  MOCK_METHOD(void, writeBlob, (const Photo::Id &, const QString& bt, const QByteArray &), (override));
  MOCK_METHOD(QByteArray, readBlob, (const Photo::Id &, const QString& bt), (override));
  MOCK_METHOD(std::vector<Photo::Id>, markStagedAsReviewed, (), (override));
  MOCK_METHOD(Database::BackendStatus, init, (const Database::ProjectInfo &), (override));
  MOCK_METHOD(void, closeConnections, (), (override));
  MOCK_METHOD(std::shared_ptr<Database::ITransaction>, openTransaction, (), (override));

  MOCK_METHOD(Database::IGroupOperator&, groupOperator, (), (override));
  MOCK_METHOD(Database::IPhotoOperator&, photoOperator, (), (override));
  MOCK_METHOD(Database::IPhotoChangeLogOperator&, photoChangeLogOperator, (), (override));
  MOCK_METHOD(Database::IPeopleInformationAccessor&, peopleInformationAccessor, (), (override));
};


#endif
