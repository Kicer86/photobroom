
#ifndef MOCK_DATABASE_HPP
#define MOCK_DATABASE_HPP

#include <gmock/gmock.h>

#include <database/idatabase.hpp>
#include <database/project_info.hpp>

//TODO: fix
struct MockDatabase: Database::IDatabase
{
    /*
    MOCK_METHOD0(notifier, Database::ADatabaseSignals*());
    MOCK_METHOD2(exec, void(std::unique_ptr<Database::IStorePhotoTask> &&, const QString &));
    MOCK_METHOD2(exec, void(std::unique_ptr<Database::IStorePhotoTask> &&, const IPhotoInfo::Ptr &));
    MOCK_METHOD2(exec, void(std::unique_ptr<Database::IStoreTagTask> &&, const TagNameInfo &));
    MOCK_METHOD1(exec, void(std::unique_ptr<Database::IListTagsTask> &&));
    MOCK_METHOD2(exec, void(std::unique_ptr<Database::IListTagValuesTask> &&, const TagNameInfo &));
    MOCK_METHOD3(exec, void(std::unique_ptr<Database::IListTagValuesTask> &&, const TagNameInfo &, const std::deque<Database::IFilter::Ptr> &));
    MOCK_METHOD1(exec, void(std::unique_ptr<Database::IGetPhotosTask> &&));
    MOCK_METHOD2(exec, void(std::unique_ptr<Database::IGetPhotosTask> &&, const std::deque<Database::IFilter::Ptr> &));
    MOCK_METHOD2(exec, void(std::unique_ptr<Database::IGetPhotoTask> &&, const IPhotoInfo::Id &));
    MOCK_METHOD2(exec, bool(std::unique_ptr<Database::IInitTask> &&, const Database::ProjectInfo &));
    MOCK_METHOD0(closeConnections, void());
    */
};

#endif
 
