
#include <gmock/gmock.h>

#include <QTemporaryDir>

#include <core/iconfiguration.hpp>
#include <core/ilogger_factory.hpp>
#include <core/ilogger.hpp>
#include "backends/sql_backends/sqlite_backend/backend.hpp"
#include "backends/sql_backends/mysql_backend/backend.hpp"
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
            QDir().mkdir(db_path);
            Database::ProjectInfo prjInfo(db_path + "/db", name);

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

        // expect db to be empty, do some checks
        db->performCustomAction([](Database::IBackendOperator* op)
        {
            const auto photos = op->getPhotos({});
            EXPECT_TRUE(photos.empty());

            const auto people = op->listPeople();
            EXPECT_TRUE(people.empty());
        });

        db->closeConnections();
    }
}


TEST_F(SqlBackendTest, personIntroduction)
{
    for(const auto& db_info: m_dbs)
    {
        const std::unique_ptr<Database::IDatabase>& db = db_info.first;
        const Database::ProjectInfo& prjInfo = db_info.second;

        db->init(prjInfo,[](const Database::BackendStatus& status)
        {
            EXPECT_EQ(status.get(), Database::StatusCodes::Ok);
        });

        db->performCustomAction([](Database::IBackendOperator* op)
        {
            const PersonName p1(Person::Id(), "P 1");
            const Person::Id p1_id = op->store(p1);

            const PersonName p1_r = op->person(p1_id);

            EXPECT_EQ(p1_r.name(), p1.name());
            EXPECT_EQ(p1_r.id(), p1_id);
        });

        db->performCustomAction([](Database::IBackendOperator* op)
        {
            const PersonName p2(Person::Id(123), "P 2");
            const Person::Id p2_id = op->store(p2);

            EXPECT_FALSE(p2_id.valid());     // p2 was nonexistent person (id 123 doesn't exist in db), we should get an error

            const PersonName p2_r = op->person(Person::Id(123));

            EXPECT_FALSE(p2_r.id().valid()); // make sure there is no entry with given id
        });

        db->performCustomAction([](Database::IBackendOperator* op)
        {
            const PersonName p2(Person::Id(), "P 2");
            const Person::Id p2_id = op->store(p2);

            const PersonName p2_dup(Person::Id(), "P 2");
            const Person::Id p2_dup_id = op->store(p2_dup);

            EXPECT_EQ(p2_dup_id, p2_id);     // we expect to get the same id in case of duplicate
        });

        db->closeConnections();
    }
}

TEST_F(SqlBackendTest, personConsistency)
{
    for(const auto& db_info: m_dbs)
    {
        const std::unique_ptr<Database::IDatabase>& db = db_info.first;
        const Database::ProjectInfo& prjInfo = db_info.second;

        db->init(prjInfo,[](const Database::BackendStatus& status)
        {
            EXPECT_EQ(status.get(), Database::StatusCodes::Ok);
        });

        db->performCustomAction([](Database::IBackendOperator* op)
        {
            std::set<Person::Id> ids;
            for(int i = 0; i < 8; i++)
            {
                const PersonName pn(Person::Id(), QString("P 3_%1").arg(i));
                const Person::Id pn_id = op->store(pn);

                ids.insert(pn_id);
            }

            const std::vector<PersonName> pns_r = op->listPeople();

            for (const PersonName& pn: pns_r)
            {
                EXPECT_TRUE(ids.find(pn.id()) != ids.end());
            }

        });

        db->closeConnections();
    }
}
