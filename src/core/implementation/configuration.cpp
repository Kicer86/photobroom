/*
 * Configuration's private implementation
 * Copyright (C) 2015  Michał Walenciak <MichalWalenciak@gmail.com>
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

#include "configuration.hpp"
#include "implementation/configuration_p.hpp"

#include <fstream>

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QVariant>

#include <system/system.hpp>

#include "constants.hpp"


ConfigurationPrivate::ConfigurationPrivate():
    m_json(),
    m_dumpTimer(),
    m_watchers()
{
    m_dumpTimer.setSingleShot(true);
    m_dumpTimer.setInterval(500);

    connect(&m_dumpTimer, &QTimer::timeout, this, &ConfigurationPrivate::saveData);

    loadData();
}


ConfigurationPrivate::~ConfigurationPrivate()
{
    if (m_dumpTimer.isActive())
    {
        m_dumpTimer.stop();
        saveData();
    }
}


QVariant ConfigurationPrivate::getEntry(const QString& entry)
{
    QVariant v_result;

    solve(entry, [&](QJsonValueRef& value)
    {
        v_result = value.toVariant();
    });

    return v_result;
}


void ConfigurationPrivate::setEntry(const QString& entry, const QVariant& entry_value)
{
    solve(entry, [&](QJsonValueRef& value)
    {
        value = QJsonValue::fromVariant(entry_value);
    });

    const auto w_it = m_watchers.find(entry);

    if (w_it != m_watchers.end())
        for (const IConfiguration::Watcher& watcher: w_it->second)
            watcher(entry, entry_value);

    markDataDirty();
}


void ConfigurationPrivate::watchFor(const QString& key, const IConfiguration::Watcher& watcher)
{
    m_watchers[key].push_back(watcher);
}


void ConfigurationPrivate::loadData()
{
    const QString path = System::getApplicationConfigDir();
    const QString configFilePath = path + "/" + "config.json";

    if (QFile::exists(configFilePath))
    {
        QFile configFile(configFilePath);
        configFile.open(QIODevice::ReadOnly);

        const QByteArray configFileContent = configFile.readAll();
        const QJsonDocument jsonDoc = QJsonDocument::fromJson(configFileContent);

        auto locked_config = m_json.lock();
        *locked_config = jsonDoc.object();
    }
    else
    {
        //load default data
    }
}


void ConfigurationPrivate::markDataDirty()
{
    m_dumpTimer.start();
}


void ConfigurationPrivate::saveData()
{
    const QString path = System::getApplicationConfigDir();
    const QString configFilePath = path + "/" + "config.json";
    const QFileInfo configPathInfo(configFilePath);
    const QDir configDir(configPathInfo.absolutePath());

    if (configDir.exists() == false)
        configDir.mkpath(".");

    auto locked_config = m_json.lock();

    QJsonDocument jsonDoc(*locked_config);

    QFile configFile(configFilePath);

    configFile.open(QIODevice::WriteOnly);
    configFile.write(jsonDoc.toJson());
}


void ConfigurationPrivate::solve(const QString& entry, std::function<void(QJsonValueRef &)> operation)
{
    if (entry.isEmpty() == false)
    {
        auto config = m_json.lock();

        const QStringList config_entries = entry.split("::");

        std::function<void(QJsonObject& jsonObj, const QStringList& levels)> traverser;

        traverser = [&operation, &traverser](QJsonObject& jsonObj, QStringList entries) -> void
        {
            const QString& name = entries.front();
            QJsonValueRef value = jsonObj[name];

            if (entries.size() == 1)
            {
                assert(value.isObject() == false);
                assert(value.isUndefined() == false);
                assert(value.isNull() == false);

                operation(value);
            }
            else
            {
                assert(value.isObject());
                QJsonObject obj = value.toObject();
                entries.pop_front();

                traverser(obj, entries);

                value = obj;
            }
        };

        traverser(*config, config_entries);
    }
}


////////////////////////////////////////////////////////////////////////////////


Configuration::Configuration():
    d(new ConfigurationPrivate)
{

}


Configuration::~Configuration()
{
    delete d;
}


QVariant Configuration::getEntry(const QString& entry)
{
    return d->getEntry(entry);
}


void Configuration::setEntry(const QString& entry, const QVariant& value)
{
    d->setEntry(entry, value);
}


void Configuration::setDefaultValue(const QString& entry, const QVariant& value)
{
    const QVariant curren_value = getEntry(entry);
    if (curren_value.isNull())
        setEntry(entry, value);
}


void Configuration::watchFor(const QString& key, const Watcher& watcher)
{
    d->watchFor(key, watcher);
}
