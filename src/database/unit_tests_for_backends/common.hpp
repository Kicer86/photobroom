
#ifndef COMMON_HPP
#define COMMON_HPP

// Some fakes, mocks and stubs

#include <QTemporaryDir>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "backends/sql_backends/sqlite_backend/backend.hpp"
#include "backends/memory_backend/memory_backend.hpp"
#include "project_info.hpp"
#include "unit_tests_utils/empty_logger.hpp"


namespace
{
    template<typename T>
    std::unique_ptr<T> construct(ILogger *);

    template<>
    std::unique_ptr<Database::SQLiteBackend> construct<Database::SQLiteBackend>(ILogger* logger)
    {
        return std::make_unique<Database::SQLiteBackend>(nullptr, logger);
    }

    template<>
    std::unique_ptr<Database::MemoryBackend> construct<Database::MemoryBackend>(ILogger *)
    {
        return std::make_unique<Database::MemoryBackend>();
    }


    template<typename T>
    struct BackendInfo;

    template<>
    struct BackendInfo<Database::SQLiteBackend>
    {
        static constexpr const char* name = "SQLite";
    };

    template<>
    struct BackendInfo<Database::MemoryBackend>
    {
        static constexpr const char* name = "Memory";
    };
}


template<typename T>
struct DatabaseTest: testing::Test
{
    DatabaseTest()
        : testing::Test()
        , m_backend(construct<T>(&m_logger))
    {
        const QString name = BackendInfo<T>::name;
        const QString wd = m_wd.path();
        const QString db_path = wd + "/" + name;
        QDir().mkdir(db_path);
        Database::ProjectInfo prjInfo(db_path + "/db", name);

        EXPECT_TRUE(m_backend->init(prjInfo));
    }

    ~DatabaseTest()
    {
        m_backend->closeConnections();;
    }

    EmptyLogger m_logger;
    QTemporaryDir m_wd;
    std::unique_ptr<Database::IBackend> m_backend;
};

using BackendTypes = testing::Types<Database::SQLiteBackend, Database::MemoryBackend>;

#endif
