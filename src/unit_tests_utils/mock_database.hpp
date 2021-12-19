
#ifndef MOCK_DATABASE_HPP
#define MOCK_DATABASE_HPP

#include <gmock/gmock.h>

#include <database/idatabase.hpp>
#include <database/project_info.hpp>


// depends on: https://github.com/google/googletest/issues/395
struct MockDatabase: Database::IDatabase
{
    MOCK_METHOD(void, update, (const Photo::DataDelta &), (override) );

    MOCK_METHOD(Database::IUtils&, utils, (), (override));
    MOCK_METHOD(Database::IBackend&, backend, (), (override));

    MOCK_METHOD(void, execute, (std::unique_ptr<Database::IDatabase::ITask> &&), (override));

    MOCK_METHOD(void, init, (const Database::ProjectInfo &, const Callback<const Database::BackendStatus &> &), (override) );

    MOCK_METHOD(void, closeConnections, (), (override));
};

#endif

