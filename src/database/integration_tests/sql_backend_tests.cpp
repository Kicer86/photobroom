
#include <gmock/gmock.h>

#include <QTemporaryDir>

#include <core/iconfiguration.hpp>
#include <core/ilogger_factory.hpp>
#include <core/ilogger.hpp>
#include "backends/sql_backends/sqlite_backend/backend.hpp"
#include "database_builder.hpp"
#include "plugins/iplugin_loader.hpp"
#include "project_info.hpp"

namespace   // Some fakes, mocks and stubs
{
    struct PluginLoader: IPluginLoader
    {
        PluginLoader()
        {
            m_plugins.push_back(&m_sqlitePlugin);
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
    };

    struct Logger: ILogger
    {
        void debug(const std::string &) override {}
        void error(const std::string &) override {}
        void info(const std::string &) override {}
        void log(ILogger::Severity , const std::string &) override {}
        void warning(const std::string &) override {}
    };

    struct LoggerFactory: ILoggerFactory
    {
        std::unique_ptr<ILogger> get(const QString &) const override
        {
            return std::make_unique<Logger>();
        }

        std::unique_ptr<ILogger> get(const QStringList &) const override
        {
            return std::make_unique<Logger>();
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
}

struct SqlBackendTest: testing::Test
{
    SqlBackendTest()
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
            Database::ProjectInfo prjInfo(db_path, name);

            auto db = builder.get(prjInfo);

            m_dbs.emplace_back(std::move(db), prjInfo);
        }
    }

    ~SqlBackendTest()
    {
    }

    PluginLoader m_loader;
    LoggerFactory m_logger;
    Config m_config;

    QTemporaryDir m_wd;
    typedef std::pair<std::unique_ptr<Database::IDatabase>, Database::ProjectInfo> DBInfo;
    std::vector<DBInfo> m_dbs;
};


TEST_F(SqlBackendTest, opening)
{
    for(const auto& db_info: m_dbs)
    {
        const std::unique_ptr<Database::IDatabase>& db = db_info.first;
        const Database::ProjectInfo& prjInfo = db_info.second;

        db->init(prjInfo,[](const Database::BackendStatus& status)
        {
            EXPECT_EQ(status.get(), Database::StatusCodes::Ok);
        });

        db->closeConnections();
    }
}
