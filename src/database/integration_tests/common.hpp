
// Some fakes, mocks and stubs

#include <QTemporaryDir>

#include <core/iconfiguration.hpp>
#include <core/ilogger.hpp>
#include <core/ilogger_factory.hpp>

#include <unit_tests_utils/empty_logger.hpp>
#include "backends/sql_backends/sqlite_backend/backend.hpp"
#include "backends/sql_backends/mysql_backend/backend.hpp"
#include "database_builder.hpp"
#include "plugins/iplugin_loader.hpp"
#include "project_info.hpp"

namespace Tests
{
    struct PluginLoader: IPluginLoader
    {
        PluginLoader()
        {
            m_plugins.push_back(&m_sqlitePlugin);
            //m_plugins.push_back(&m_mysqlPlugin);
        }

        Database::IPlugin* getDBPlugin(const QString& name) override
        {
            auto it = std::find_if(m_plugins.begin(), m_plugins.end(), [name](Database::IPlugin* plugin)
            {
                return plugin->backendName() == name;
            });

            return *it;
        }

        const std::vector<Database::IPlugin *>& getDBPlugins() const override
        {
            return m_plugins;
        }

        std::vector<Database::IPlugin *> m_plugins;
        Database::SQLitePlugin m_sqlitePlugin;
        Database::MySqlPlugin m_mysqlPlugin;
    };

    struct LoggerFactory: ILoggerFactory
    {
        std::unique_ptr<ILogger> get(const QString &) const override
        {
            return std::make_unique<EmptyLogger>();
        }

        std::unique_ptr<ILogger> get(const QStringList &) const override
        {
            return std::make_unique<EmptyLogger>();
        }
    };

    struct Config: IConfiguration
    {
        QVariant getEntry(const QString & ) override
        {
            return QVariant();
        }

        void setDefaultValue(const QString &, const QVariant &) override {}
        void setEntry(const QString &, const QVariant &) override {}
        void watchFor(const QString &, const IConfiguration::Watcher &) override {}
    };


    struct DatabaseTest: testing::Test
    {
        DatabaseTest()
        {
            // Setup builder
            Database::Builder builder;
            builder.set(&m_loader);
            builder.set(&m_logger);
            builder.set(&m_config);

            const QString wd = m_wd.path();
            const auto plugins = m_loader.getDBPlugins();

            // prepare db for each plugin
            for (const Database::IPlugin* plugin: plugins)
            {
                const QString name = plugin->backendName();
                const QString db_path = wd + "/" + name;
                QDir().mkdir(db_path);
                Database::ProjectInfo prjInfo(db_path + "/db", name);

                auto db = builder.get(prjInfo);

                m_dbs.emplace_back(std::move(db), prjInfo);
            }
        }

        ~DatabaseTest()
        {
        }

        template<typename C>
        void for_all_db_plugins(C&& c)
        {
            for(const auto& db_info: m_dbs)
            {
                const std::unique_ptr<Database::IDatabase>& db = db_info.first;
                const Database::ProjectInfo& prjInfo = db_info.second;

                db->init(prjInfo,[](const Database::BackendStatus& status)
                {
                    EXPECT_EQ(status.get(), Database::StatusCodes::Ok);
                });

                c(db.get());

                db->closeConnections();
            }
        }

        Tests::PluginLoader m_loader;
        Tests::LoggerFactory m_logger;
        Tests::Config m_config;

        QTemporaryDir m_wd;
        typedef std::pair<std::unique_ptr<Database::IDatabase>, Database::ProjectInfo> DBInfo;
        std::vector<DBInfo> m_dbs;
    };
}
