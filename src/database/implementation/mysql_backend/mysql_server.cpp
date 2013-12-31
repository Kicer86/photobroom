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

#include "mysql_server.hpp"

#include <fstream>

#include <QProcess>

#include <boost/optional.hpp>
#include <boost/filesystem.hpp>

#include "configuration/configurationfactory.hpp"
#include "configuration/iconfiguration.hpp"
#include "configuration/entrydata.hpp"
#include "system/system.hpp"

#include "databasebuilder.hpp"

namespace
{
    const char* MySQL_daemon = "Database::Backend::MySQL::Server";

    const char* MySQL_config =
    "#                                                                                            \n"
    "# Global Akonadi MySQL server settings,                                                      \n"
    "# These settings can be adjusted using $HOME/.config/akonadi/mysql-local.conf                \n"
    "#                                                                                            \n"
    "# Based on advice by Kris Köhntopp <kris@mysql.com>                                          \n"
    "#                                                                                            \n"
    "[mysqld]                                                                                     \n"

    "# strict query parsing/interpretation                                                        \n"
    "# TODO: make Akonadi work with those settings enabled                                        \n"
    "# sql_mode=strict_trans_tables,strict_all_tables,strict_error_for_division_by_zero,no_auto_create_user,no_auto_value_on_zero,no_engine_substitution,no_zero_date,no_zero_in_date,only_full_group_by,pipes_as_concat\n"
    "# sql_mode=strict_trans_tables                                                               \n"

    "# DEBUGGING:\n"
    "# log all queries, useful for debugging but generates an enormous amount of data\n"
    "# log=mysql.full\n"
    "# log queries slower than n seconds, log file name relative to datadir (for debugging only)\n"
    "# log_slow_queries=mysql.slow\n"
    "# long_query_time=1\n"
    "# log queries not using indices, debug only, disable for production use\n"
    "# log_queries_not_using_indexes=1\n"
    "#\n"
    "# mesure database size and adjust innodb_buffer_pool_size\n"
    "# SELECT sum(data_length) as bla, sum(index_length) as blub FROM information_schema.tables WHERE table_schema not in (\"mysql\", \"information_schema\");\n"

    "# NOTES:\n"
    "# Keep Innob_log_waits and keep Innodb_buffer_pool_wait_free small (see show global status like \"inno%\", show global variables)\n"

    "#expire_logs_days=3\n"

    "#sync_bin_log=0\n"

    "# Use UTF-8 encoding for tables\n"
    "character_set_server=utf8\n"
    "collation_server=utf8_general_ci\n"

    "# use InnoDB for transactions and better crash recovery\n"
    "default_storage_engine=innodb\n"

    "# memory pool InnoDB uses to store data dictionary information and other internal data structures (default:1M)\n"
    "# Deprecated in MySQL >= 5.6.3\n"
    "innodb_additional_mem_pool_size=1M\n"

    "# memory buffer InnoDB uses to cache data and indexes of its tables (default:128M)\n"
    "# Larger values means less I/O\n"
    "innodb_buffer_pool_size=80M\n"

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

    "# Maximum size of one packet or any generated/intermediate string. (default:1M)\n"
    "max_allowed_packet=32M\n"

    "# Maximum simultaneous connections allowed (default:100)\n"
    "max_connections=256\n"

    "# The two options below make no sense with prepared statements and/or transactions\n"
    "# (make sense when having the same query multiple times)\n"

    "# Memory allocated for caching query results (default:0 (disabled))\n"
    "query_cache_size=0\n"

    "# Do not cache results (default:1)\n"
    "query_cache_type=0\n"

    "# Do not use the privileges mechanisms\n"
    "skip_grant_tables\n"

    "# Do not listen for TCP/IP connections at all\n"
    "skip_networking\n"

    "# The number of open tables for all threads. (default:64)\n"
    "table_open_cache=200\n"

    "# How many threads the server should cache for reuse (default:0)\n"
    "thread_cache_size=3\n"

    "# wait 365d before dropping the DB connection (default:8h)\n"
    "wait_timeout=31536000\n"

    "[client]\n"
    "default-character-set=utf8\n";

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
}


MySqlServer::MySqlServer(): m_serverProcess(new QProcess)
{

}



MySqlServer::~MySqlServer()
{

}


bool MySqlServer::run_server()
{
    bool status = false;
    const std::string path = getDaemonPath();

    if (path.empty() == false)
    {
        std::shared_ptr<IConfiguration> config = ConfigurationFactory::get();
        boost::optional<Configuration::EntryData> dbPath = config->findEntry(Database::databaseLocation);

        if (dbPath)
        {
            const std::string basePath = dbPath->value() + "/MySQL/";
            const std::string configFile = basePath + "mysql.conf";
            const std::string baseDataPath = basePath + "db_data";

            if (boost::filesystem::exists(configFile) == false)
            {
                std::ofstream os(configFile);

                os << MySQL_config;
            }

            const std::string mysql_config  = "--defaults-file=" + configFile;
            const std::string mysql_datadir = "--datadir=" + baseDataPath;
            const std::string mysql_socket  = "--socket=" + basePath + "mysql.socket";

            if (boost::filesystem::exists(baseDataPath) == false)
                status = boost::filesystem::create_directories(baseDataPath);

            if (status)
            {
                QStringList args = { mysql_config.c_str(), mysql_datadir.c_str(), mysql_socket.c_str() };

                m_serverProcess->setProgram(path.c_str());
                m_serverProcess->setArguments(args);
                m_serverProcess->closeWriteChannel();

                std::cout << "Starting: " << path << " " << args.join(" ").toStdString() << std::endl;

                m_serverProcess->start();

                status = m_serverProcess->waitForStarted();
            }
        }
    }

    return status;
}


std::string MySqlServer::getDaemonPath() const
{
    //get path to server
    std::shared_ptr<IConfiguration> config = ConfigurationFactory::get();

    boost::optional<Configuration::EntryData> daemonPath = config->findEntry(MySQL_daemon);

    std::string path;
    if (daemonPath)
        path = daemonPath->value();
    else
    {
        path = System::findProgram("mysqld");

        Configuration::EntryData mysqldPath(MySQL_daemon, path);
        config->addEntry(mysqldPath);
    }

    return path;
}


