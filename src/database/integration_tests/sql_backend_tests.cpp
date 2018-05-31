
#include <gmock/gmock.h>

#include "plugins/iplugin_loader.hpp"
#include "database/backends/sql_backends/sqlite_backend/backend.hpp"

namespace
{
    struct PluginLoader: IPluginLoader
    {
        PluginLoader()
        {
            m_plugins.push_back(&m_sqlitePlugin);
        }

        Database::IPlugin* getDBPlugin(const QString &) override
        {
            return nullptr;
        }

        const std::vector<Database::IPlugin *>& getDBPlugins() const override
        {
            return m_plugins;
        }

        std::vector<Database::IPlugin *> m_plugins;
        Database::SQLitePlugin m_sqlitePlugin;
    };
}

struct SqlBackendTest: testing::Test
{
    SqlBackendTest()
    {


    }

    ~SqlBackendTest()
    {
    }
};
