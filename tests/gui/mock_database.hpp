
#ifndef MOCK_DATABASE_HPP
#define MOCK_DATABASE_HPP

#include <gmock/gmock.h>

#include <database/idatabase.hpp>
#include <database/project_info.hpp>

struct MockDatabase: Database::IDatabase
{
   
    MOCK_METHOD2(addPath, void(const Database::Task &, const QString &));
    MOCK_METHOD0(closeConnections, void());
    MOCK_METHOD1(getAllPhotos, void(const Database::Task &));
    MOCK_METHOD2(getPhoto, void(const Database::Task &, const IPhotoInfo::Id &));
    MOCK_METHOD2(getPhotos, void(const Database::Task &, const std::deque<Database::IFilter::Ptr> &));
    MOCK_METHOD2(init, bool(const Database::Task &, const Database::ProjectInfo &));
    MOCK_METHOD1(listTags, void(const Database::Task &));
    MOCK_METHOD3(listTagValues, void(const Database::Task &, const TagNameInfo &, const std::deque<Database::IFilter::Ptr> &));
    MOCK_METHOD2(listTagValues, void(const Database::Task &, const TagNameInfo &));
    MOCK_METHOD0(notifier, Database::ADatabaseSignals*());
    MOCK_METHOD1(prepareTask, Database::Task(Database::IDatabaseClient *));
    MOCK_METHOD2(update, void(const Database::Task &, const IPhotoInfo::Ptr &));
};

#endif
 
