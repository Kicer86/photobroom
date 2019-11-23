
#include <cassert>
#include <iostream>
#include <string>
#include <vector>

#include <QCoreApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QTimer>

#ifdef OS_WIN
#include <Windows.h>
#endif

#include <core/configuration.hpp>
#include <core/core_factory_accessor.hpp>
#include <core/exif_reader_factory.hpp>
#include <core/features_manager.hpp>
#include <core/logger_factory.hpp>
#include <core/python_thread.hpp>
#include <core/task_executor.hpp>
#include <core/ilogger.hpp>
#include <crash_catcher/crash_catcher.hpp>
#include <database/database_builder.hpp>
#include <gui/gui.hpp>
#include <plugins/plugin_loader.hpp>
#include <project_utils/project_manager.hpp>
#include <system/system.hpp>

#include "paths.hpp"


enum class CrashCatcherStatus
{
    Disabled,
    Ok,
    Error,
};


namespace
{
    class ConfigStorage: public IConfigStorage
    {
    public:
        ConfigStorage(const QString& configFile): m_configFile(configFile) {}

        IConfigStorage::Content load() override
        {
            IConfigStorage::Content config;

            if (QFile::exists(m_configFile))
            {
                QFile configFile(m_configFile);
                configFile.open(QIODevice::ReadOnly);

                const QByteArray configFileContent = configFile.readAll();
                const QJsonDocument jsonDoc = QJsonDocument::fromJson(configFileContent);
                const QJsonObject configJsonObj = jsonDoc.object();

                config = readNode(configJsonObj);

            }
            else
            {
                //load default data
            }

            return config;
        }

        void save(const IConfigStorage::Content& configuration) override
        {
            const QFileInfo configPathInfo(m_configFile);
            const QDir configDir(configPathInfo.absolutePath());

            if (configDir.exists() == false)
                configDir.mkpath(".");

            QJsonObject configurationObject;

            for(const auto& [key, value]: configuration)
            {
                const QStringList entries = key.split("::");
                writeTo(configurationObject, entries, value);
            }

            QJsonDocument jsonDoc(configurationObject);

            QFile configFile(m_configFile);

            configFile.open(QIODevice::WriteOnly);
            configFile.write(jsonDoc.toJson());
        }

    private:
        QString m_configFile;

        void writeTo(QJsonObject& obj, QStringList configPath, const QVariant& value)
        {
            const QString entryName = configPath.front();

            if (configPath.size() == 1)
            {
                const QJsonValue json_value = QJsonValue::fromVariant(value);
                assert(json_value.isUndefined() == false);
                assert(json_value.isNull() == false);

                obj[entryName] = json_value;
            }
            else
            {
                configPath.takeFirst();
                QJsonValueRef jsonValue = obj[entryName];
                assert(jsonValue.isNull() || jsonValue.isObject());

                QJsonObject subObj = jsonValue.toObject();
                writeTo(subObj, configPath, value);

                jsonValue = subObj;
            }
        }

        IConfigStorage::Content readNode(const QJsonObject& obj, const QString& entry_namespace = {})
        {
            IConfigStorage::Content content;

            for(auto it = obj.constBegin(); it != obj.constEnd(); ++it)
            {
                const QString name = it.key();
                const QJsonValue value = it.value();

                assert(value.isArray() == false);
                assert(value.isNull() == false);

                const QString sub_namespace = entry_namespace + (entry_namespace.isEmpty()? name : QString("::%1").arg(name));

                if (value.isObject())
                {
                    const QJsonObject sub_obj = value.toObject();
                    const IConfigStorage::Content sub_content = readNode(sub_obj, sub_namespace);

                    content.insert(sub_content.cbegin(), sub_content.cend());
                }
                else
                    content.emplace(sub_namespace, value);
            }

            return content;
        }
    };
}


int main(int argc, char **argv)
{
    Gui gui(argc, argv);

    QCoreApplication* app = gui.getApp();
    app->setApplicationName("photo_broom");                                // without this app name may change when binary name changes

    const QString basePath = System::getApplicationConfigDir();

    // setup internal locations
    QDir::setSearchPaths("icons", {QString(Paths::icons) + "/svg"});

    // perform command line parsing
    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption logingLevelOption("loging-level",
                                         QCoreApplication::translate("main", "Defines loging level. Possible options are: Debug, Info, Warning (default), Error"),
                                         QCoreApplication::translate("main", "loging level"),
                                         "Warning"
                       );

    QCommandLineOption crashTestOption("test-crash-catcher", "When specified, photo_broom will crash 3 seconds after being launch");
    crashTestOption.setFlags(QCommandLineOption::HiddenFromHelp);

    QCommandLineOption disableCrashCatcher("disable-crash-catcher", "Turns off crash catcher");

#ifdef OS_WIN
    QCommandLineOption enableConsole("enable-console", "Opens console with app's output messages");
    enableConsole.setHidden(true);

    parser.addOption(enableConsole);
#endif

    parser.addOption(logingLevelOption);
    parser.addOption(crashTestOption);
    parser.addOption(disableCrashCatcher);

    parser.process(*app);

    const bool enableCrashTest = parser.isSet(crashTestOption);
    if (enableCrashTest)
        QTimer::singleShot(3000, []
        {
            int* ptr = nullptr;
            volatile int v = *ptr;
            (void) v;
        });

    const QString logingLevelStr = parser.value(logingLevelOption);
    ILogger::Severity logingLevel = ILogger::Severity::Warning;

    if (logingLevelStr == "Debug")
        logingLevel = ILogger::Severity::Debug;
    else if (logingLevelStr == "Info")
        logingLevel = ILogger::Severity::Info;
    else if (logingLevelStr == "Warning")
        logingLevel = ILogger::Severity::Warning;
    else if (logingLevelStr == "Error")
        logingLevel = ILogger::Severity::Error;
    else
    {
        std::cerr << "Invalid option: '" << logingLevelStr.toStdString() << "' for loging-level option" << std::endl;
        return 1;
    }

    const bool crashCatcherDisabled = parser.isSet(disableCrashCatcher);

#ifdef OS_WIN
    if (parser.isSet(enableConsole) && (AttachConsole(ATTACH_PARENT_PROCESS) || AllocConsole()) )
    {
        freopen("CONOUT$", "w", stdout);
        freopen("CONOUT$", "w", stderr);
    }
#endif

    // build objects
    CrashCatcherStatus status = CrashCatcherStatus::Disabled;

    if (crashCatcherDisabled == false)
        status = CrashCatcher::init(argv[0]) ? CrashCatcherStatus::Ok : CrashCatcherStatus::Error;

    LoggerFactory logger_factory(basePath);
    logger_factory.setLogingLevel(logingLevel);

    FeaturesManager featuresManager(&logger_factory);

    const QString configFileDir = System::getApplicationConfigDir();
    const QString configFilePath = configFileDir + "/" + "config.json";
    ConfigStorage configStorage(configFilePath);
    Configuration configuration(configStorage);

    PythonThread pythonThread;

    PluginLoader pluginLoader;
    pluginLoader.set(&logger_factory);

    auto taskExecutorLogger = logger_factory.get("TaskExecutor");
    TaskExecutor taskExecutor(taskExecutorLogger.get());

    Database::Builder database_builder;
    database_builder.set(&pluginLoader);
    database_builder.set(&logger_factory);
    database_builder.set(&configuration);

    ProjectManager prjManager(&database_builder);

    switch (status)
    {
        case CrashCatcherStatus::Ok:
            logger_factory.get("CrashCatcher")->info("Initialization successful");
            break;

        case CrashCatcherStatus::Error:
            logger_factory.get("CrashCatcher")->error("Initialization failed");
            break;

        case CrashCatcherStatus::Disabled:
            logger_factory.get("CrashCatcher")->info("Disabled");
            break;
    }

    ExifReaderFactory exifReaderFactory;

    CoreFactoryAccessor coreFactory(&logger_factory,
                                    &exifReaderFactory,
                                    &configuration,
                                    &taskExecutor,
                                    &pythonThread,
                                    &featuresManager
    );

    // start gui
    gui.set(&prjManager);
    gui.set(&pluginLoader);
    gui.set(&coreFactory);
    gui.run();

    taskExecutor.stop();

    System::cleanTemporaries();

    return 0;
}
