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


#include "default_configuration.hpp"

#include <QString>

#include "system/system.hpp"

#include "entrydata.hpp"
#include "default_configuration_private.hpp"


DefaultConfiguration::DefaultConfiguration(): m_impl(new DefaultConfigurationPrivate)
{
    //load static data
    loadXml(":/config/base_config.xml");

    //load dynamic data
    std::vector<Configuration::EntryData> defaultEntries =
    {
        Configuration::EntryData(Configuration::configLocation, m_impl->getConfigDir()),
    };

    registerDefaultEntries(defaultEntries);

    std::cout << "DefaultConfiguration: using " << m_impl->getConfigDir() << " "
              << "for broom's base dir" << std::endl;
}


DefaultConfiguration::~DefaultConfiguration()
{

}


boost::optional<Configuration::EntryData> DefaultConfiguration::findEntry(const Configuration::ConfigurationKey& key) const
{
    return m_impl->find(key);
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


bool DefaultConfiguration::loadXml(const QString& path)
{
    return m_impl->loadXml(path);
}


bool DefaultConfiguration::useXml(const std::string& xml)
{
    return m_impl->useXml(xml.c_str());
}

