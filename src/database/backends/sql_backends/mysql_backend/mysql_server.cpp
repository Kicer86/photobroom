/*
 * A class for managing mysql server process
 * Copyright (C) 2013  Michał Walenciak <MichalWalenciak@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

/// TODO: remove
#if defined _MSC_VER 
	#if _MSC_VER >= 1800
		#define Q_COMPILER_INITIALIZER_LISTS
	#else
		#error unsupported compiler
	#endif
#endif


#include "mysql_server.hpp"

#include <QProcess>
#include <QFile>
#include <QDir>
#include <QTimer>
#include <QFileSystemWatcher>
#include <QEventLoop>

#include <configuration/iconfiguration.hpp>
#include <configuration/entrydata.hpp>
#include <core/ilogger.hpp>
#include <core/disk_observer.hpp>
#include <database/database_builder.hpp>
#include <system/system.hpp>


namespace
{
    const char* MySQL_daemon = "Database::Backend::MySQL::Server";

    const char* MySQL_config =
        "[mysqld]                                                                                     \n"
        "# Use UTF-8 encoding for tables\n"
        "character_set_server=utf8\n"
        "collation_server=utf8_general_ci\n"

        "# use InnoDB for transactions and better crash recovery\n"
        "default_storage_engine=innodb\n"

        "# memory pool InnoDB uses to store data dictionary information and other internal data structures (default:1M)\n"
        "# Deprecated in MySQL >= 5.6.3\n"
        "innodb_additional_mem_pool_size=1M\n"

        "# Create a .ibd file for each table (default:0)\n"
        "innodb_file_per_table=1\n"

        "# Write out the log buffer to the log file at each commit (default:1)\n"
        "innodb_flush_log_at_trx_commit=2\n"

        "# Buffer size used to write to the log files on disk (default:1M for builtin, 8M for plugin)\n"
        "# larger values means less I/O\n"
        "innodb_log_buffer_size=1M\n"

        "# Size of each log file in a log group (default:5M) larger means less I/O but more time for recovery.\n"
        "innodb_log_file_size=64M\n"

        "# # error log file name, relative to datadir (default:hostname.err)\n"
        "log_error=mysql.err\n"

        "# print warnings and connection errors (default:1)\n"
        "log_warnings=2\n"

        "# Convert table named to lowercase\n"
        "lower_case_table_names=1\n"

        "# Do not use the privileges mechanisms\n"
        "skip_grant_tables\n"

        "# Do not listen for TCP/IP connections at all\n"
        "skip_networking\n"

        "[client]\n"
        "default-character-set=utf8\n";

        /*
    struct MySqlServerInit: public Configuration::IInitializer
    {

        MySqlServerInit()
        {
            std::shared_ptr<IConfiguration> config = ConfigurationFactory::get();

            config->registerInitializer(this);
        }

        virtual std::string getXml()
        {
            const std::string configuration_xml =
                "<configuration>                                        "
                "    <keys>                                             "
                "        <key name='" + std::string(MySQL_daemon) + "' />            "
                "    </keys>                                            "
                "</configuration>                                       ";

            return configuration_xml;
        }

    } init;
    */
}


/*******************************************************************************************/

MySqlServer::MySqlServer(): m_serverProcess(nullptr), m_configuration(nullptr), m_logger(nullptr)
{

}


MySqlServer::~MySqlServer()
{
    if (m_serverProcess)
    {
        m_logger->log({"Database", "MySQL"}, ILogger::Severity::Info, "closing down MySQL server");

        m_serverProcess->terminate();
        m_serverProcess->waitForFinished();

        m_logger->log({"Database", "MySQL"}, ILogger::Severity::Info, "MySQL server down");
    }
    else
        m_logger->log({"Database", "MySQL"}, ILogger::Severity::Info, "MySQL server not owned by photo broom. Leaving it untouched");
}


QString MySqlServer::run_server(const QString& basePath)
{
    QString result = "";
    const QString path = getDaemonPath();

    if (path.isEmpty() == false)
        result = startProcess(path, basePath);

    return result;
}


void MySqlServer::set(IConfiguration* configuration)
{
    m_configuration = configuration;
}


void MySqlServer::set(ILogger* logger)
{
    m_logger = logger;
}


QString MySqlServer::getDaemonPath() const
{
    //get path to server
    ol::Optional<QString> daemonPath = m_configuration->findEntry(MySQL_daemon);

    QString path;

    if (daemonPath)
        path = *daemonPath;

    if (path.isEmpty())
    {
        path = System::findProgram("mysqld").c_str();

        Configuration::EntryData mysqldPath(MySQL_daemon, path);
        m_configuration->addEntry(mysqldPath);
    }

    return path;
}


bool MySqlServer::initDB(const std::string& dbDir, const std::string& extraOptions) const
{
    const std::string path = System::findProgram("mysql_install_db");
    bool status = false;

    if (path.empty() == false)
    {
        QProcess init;

        const std::string userName = System::userName();
        const std::string dataDirOption  = "--datadir=" + dbDir;
        const std::string userNameOption = "--user=" + userName;

        const QString installDBPath = path.c_str();
        const QFileInfo installDBPathInfo(installDBPath);
        QDir installDBDir = installDBPathInfo.absoluteDir();
        installDBDir.cdUp();

        init.setWorkingDirectory(installDBDir.path());
        init.start( installDBPath, {dataDirOption.c_str(), userNameOption.c_str(), extraOptions.c_str()} );
        status = init.waitForStarted();
        init.waitForFinished();

        status = init.exitCode() == QProcess::NormalExit;

        if (!status)
        {
            m_logger->log({"Database", "MySQL"},
                          ILogger::Severity::Error,
                          "MySQL Database Backend: database initialization failed:" + QString(init.readAll()).toStdString() );

            status = QDir(dbDir.c_str()).rmdir(dbDir.c_str());
        }
    }

    return status;
}


bool MySqlServer::createConfig(const QString& configFile) const
{
    bool status = true;

    if (QFile::exists(configFile) == false)
    {
        QFile os(configFile);

        status = os.open(QFile::WriteOnly);

        if (status)
            status = os.write(MySQL_config) != -1;
    }

    return status;
}


bool MySqlServer::waitForServerToStart(const QString& socketPath) const
{
    //wait for socket to appear
    std::string logMsg = "Waiting for MySQL server to get up: ";

    DiskObserver observer;
    const bool status = observer.waitForFileToAppear(socketPath);

    if (status)
        logMsg += "done.";
    else
        logMsg += "timeout error.";

    m_logger->log({"Database", "MySQL"}, ILogger::Severity::Info, logMsg);

    return status;
}


QString MySqlServer::startProcess(const QString& daemonPath, const QString& basePath)
{
    const QString socketPath = basePath + "mysql.socket";
    const bool alive = QFile::exists(socketPath);
    bool status = true;

    if (!alive)
    {
        const QString configFile = basePath + "mysql.conf";
        const QString baseDataPath = basePath + "db_data";
        const QString mysql_config  = "--defaults-file=" + configFile;
        const QString mysql_datadir = "--datadir=" + baseDataPath;
        const QString mysql_socket  = "--socket=" + socketPath;

        if (status)
            status = createConfig(configFile);

        if (status)
            if (QDir(baseDataPath).exists() == false)
                status = initDB(baseDataPath.toStdString(), mysql_config.toStdString());

        if (status)
        {
            QStringList args = { mysql_config, mysql_datadir, mysql_socket};

            m_serverProcess.reset(new QProcess);
            m_serverProcess->setProgram(daemonPath);
            m_serverProcess->setArguments(args);
            m_serverProcess->closeWriteChannel();

            m_logger->log({"Database", "MySQL"},
                          ILogger::Severity::Debug,
                          "MySQL Database Backend: " + daemonPath.toStdString() + " " + args.join(" ").toStdString());

            m_serverProcess->start();
            status = m_serverProcess->waitForStarted();

            if (status)
                status = waitForServerToStart(socketPath);
        }

    }

    const QString result = status? socketPath : "";

    return result;
}
