
#ifndef MOCK_DATABASE_HPP
#define MOCK_DATABASE_HPP

#include <gmock/gmock.h>

#include <database/idatabase.hpp>
#include <database/project_info.hpp>


struct MockDatabase: Database::IDatabase
{
    MOCK_METHOD(Database::IBackend&, backend, (), (override));
    MOCK_METHOD(void, execute, (std::unique_ptr<Database::IDatabase::ITask> &&), (override));
    MOCK_METHOD(void, init, (const Database::ProjectInfo &, const Callback<const Database::BackendStatus &> &), (override) );
    MOCK_METHOD(void, closeConnections, (), (override));
};

#endif

