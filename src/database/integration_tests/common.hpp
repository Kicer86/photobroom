
// Some fakes, mocks and stubs

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
