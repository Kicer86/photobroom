
#ifndef COMMON_HPP
#define COMMON_HPP

// Some fakes, mocks and stubs

#include <QTemporaryDir>

#include "backends/sql_backends/sqlite_backend/backend.hpp"
#include "backends/sql_backends/mysql_backend/backend.hpp"
#include "backends/memory_backend/memory_backend.hpp"
#include "project_info.hpp"
#include "unit_tests_utils/empty_logger.hpp"

// @todo: use gtest's parametrized tests
namespace Tests
{

    struct DatabaseTest: testing::Test
    {
        DatabaseTest()
            : testing::Test()
            , sqlite_backend(nullptr, &logger)
            , json_backend()
        {

        }

        ~DatabaseTest()
        {
        }

        template<typename C>
        void for_all_db_plugins(C&& c)
        {
            const QString wd = m_wd.path();

            for(const auto& backend_data: m_backends)
            {
                Database::IBackend* backend = backend_data.first;
                const QString name = backend_data.second;
                const QString db_path = wd + "/" + name;
                QDir().mkdir(db_path);
                Database::ProjectInfo prjInfo(db_path + "/db", name);

                ASSERT_TRUE(backend->init(prjInfo));

                c(backend);

                backend->closeConnections();;
            }
        }

        EmptyLogger logger;
        Database::SQLiteBackend sqlite_backend;
        Database::MemoryBackend json_backend;

        QTemporaryDir m_wd;
        std::vector<std::pair<Database::IBackend *, QString>> m_backends = { {&sqlite_backend, "SQLite"}, {&json_backend, "Memory"} };
    };
}

#endif
