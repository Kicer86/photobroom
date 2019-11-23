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
#include <QJsonObject>
#include <QJsonValue>
#include <QVariant>

#include <system/system.hpp>

#include "constants.hpp"

namespace
{
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
}


ConfigurationPrivate::ConfigurationPrivate(const QString& configFile):
    m_configFile(configFile),
    m_entries(),
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
    auto config = m_entries.lock();
    const QVariant value = (*config)[entry];

    return value;
}


void ConfigurationPrivate::setEntry(const QString& entry, const QVariant& entry_value)
{
    auto config = m_entries.lock();
    (*config)[entry] = entry_value;

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
    if (QFile::exists(m_configFile))
    {
        QFile configFile(m_configFile);
        configFile.open(QIODevice::ReadOnly);

        const QByteArray configFileContent = configFile.readAll();
        const QJsonDocument jsonDoc = QJsonDocument::fromJson(configFileContent);

        auto locked_config = m_entries.lock();
        const QJsonObject configJsonObj = jsonDoc.object();
        *locked_config = readNode(configJsonObj);
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
    const QFileInfo configPathInfo(m_configFile);
    const QDir configDir(configPathInfo.absolutePath());

    if (configDir.exists() == false)
        configDir.mkpath(".");

    auto locked_config = m_entries.lock();

    QJsonObject configurationObject;

    for(const auto& [key, value]: *locked_config)
    {
        const QStringList entries = key.split("::");
        writeTo(configurationObject, entries, value);
    }

    QJsonDocument jsonDoc(configurationObject);

    QFile configFile(m_configFile);

    configFile.open(QIODevice::WriteOnly);
    configFile.write(jsonDoc.toJson());
}


////////////////////////////////////////////////////////////////////////////////


Configuration::Configuration(const QString& configFilePath):
    d(new ConfigurationPrivate(configFilePath))
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
