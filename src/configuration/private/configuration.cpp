/*
    Default Configuration class
    Copyright (C) 2013  Michał Walenciak <MichalWalenciak@gmail.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/


#include "configuration.hpp"

#include <iostream>

#include <QString>

#include <core/ilogger.hpp>
#include <system/system.hpp>

#include "entrydata.hpp"
#include "default_configuration_private.hpp"
#include "constants.hpp"


DefaultConfiguration::DefaultConfiguration(): m_impl(new DefaultConfigurationPrivate)
{

}


DefaultConfiguration::~DefaultConfiguration()
{

}


void DefaultConfiguration::init(ILogger* logger)
{
    const QString baseConfig =
    "<configuration>"

    "    <!-- introduce known configuration keys -->"
    "    <keys>"
    "        <key name='" + QString(Configuration::BasicKeys::configLocation) + "' />    <!-- base path for configurations, databases etc -->"
    "        <key name='" + QString(Configuration::BasicKeys::thumbnailWidth) + "' />"
    "    </keys>"

    "    <!-- default values -->"
    "    <defaults>"
    "        <key name='" + QString(Configuration::BasicKeys::configLocation) + "' value='" + m_impl->getConfigDir() + "' />"
    "        <key name='" + QString(Configuration::BasicKeys::thumbnailWidth) + "' value='120'/>"
    "    </defaults>"

    "</configuration>";

    registerXml(baseConfig);

    //logger->log("Configuration", ILogger::Severity::Info, "using " + m_impl->getConfigDir().toStdString() + " for broom's base dir");

    m_impl->set(logger);
}



ol::Optional<Configuration::EntryData> DefaultConfiguration::findEntry(const Configuration::ConfigurationKey& key) const
{
    return m_impl->find(key);
}


ol::Optional<QString> DefaultConfiguration::findEntry(const char* key) const
{
    const auto entry = findEntry(Configuration::ConfigurationKey(key));
    return entry->value();
}


QString DefaultConfiguration::findEntry(const char* key, const QString& defaultValue) const
{
    const auto entry = findEntry(key);
    const QString result = entry? *entry : defaultValue;

    return result;
}


const std::vector<Configuration::EntryData> DefaultConfiguration::getEntries()
{
    return m_impl->getAll();
}


void DefaultConfiguration::addEntry(const Configuration::EntryData& entry)
{
    m_impl->addEntry(entry.key(), entry);
}


void DefaultConfiguration::registerDefaultEntries(const std::vector<Configuration::EntryData>& entries)
{
    for(const auto& entry: entries)
        m_impl->addEntry(entry.key(), entry, true);
}


void DefaultConfiguration::registerKey(const Configuration::ConfigurationKey& key)
{
    m_impl->introduceKey(key);
}


void DefaultConfiguration::registerXml(const QString& xml)
{
    useXml(xml);
}


bool DefaultConfiguration::load()
{
    return m_impl->load();
}


bool DefaultConfiguration::useXml(const QString& xml)
{
    return m_impl->useXml(xml);
}

