
#ifndef MOCK_DATABASE_HPP
#define MOCK_DATABASE_HPP

#include <gmock/gmock.h>

#include <database/idatabase.hpp>
#include <database/project_info.hpp>


// depends on: https://github.com/google/googletest/issues/395
struct MockDatabase: Database::IDatabase
{
    MOCK_METHOD(void, update, (const Photo::DataDelta &), (override) );

    MOCK_METHOD2(getPhotos, void(const std::vector<Photo::Id> &, const std::function<void(const std::vector<IPhotoInfo::Ptr> &)> &) );

    MOCK_METHOD0(markStagedAsReviewed, void());

    MOCK_METHOD0(utils,   Database::IUtils&());
    MOCK_METHOD0(backend, Database::IBackend&());

    MOCK_METHOD(void, execute, (std::unique_ptr<Database::IDatabase::ITask> &&), (override));

    MOCK_METHOD2(init, void(const Database::ProjectInfo &, const Callback<const Database::BackendStatus &> &) );

    MOCK_METHOD0(closeConnections, void() );
};

#endif

