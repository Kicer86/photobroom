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

#include <stdexcept>

#include <QHash>
#include <QString>

#include "system/system.hpp"

#include "entrydata.hpp"

uint qHash(const Configuration::ConfigurationKey& key, uint seed) noexcept(noexcept(qHash(QString())))
{
    return qHash(QString(key.getKeyRaw().c_str()), seed);
}


struct DefaultConfiguration::Impl
{
    Impl(): m_data() {}
    
    std::string getConfigDir() const
    {
        const std::string result = System::getApplicationConfigDir();
        
        return result;
    }
    
    boost::optional<Configuration::EntryData> find(const Configuration::ConfigurationKey& key) const
    {
        boost::optional<Configuration::EntryData> result;
        auto it = m_data.find(key);
        
        if (it != m_data.end())
            result = it.value();
        
        return result;
    }
    
    std::vector<Configuration::EntryData> getAll() const
    {
        std::vector<Configuration::EntryData> result(m_data.begin(), m_data.end());        
        
        return result;
    }
    
    void addEntry(const Configuration::ConfigurationKey& key, const Configuration::EntryData& data)
    {
        m_data[key] = data;
    }
    
    private:
        QHash<Configuration::ConfigurationKey, Configuration::EntryData> m_data;
};


DefaultConfiguration::DefaultConfiguration(): m_impl(new Impl)
{
    std::vector<Configuration::EntryData> defaultEntries = 
    {
        Configuration::EntryData(Configuration::configLocation, m_impl->getConfigDir()),
    };
    
    registerEntries(defaultEntries);
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


void DefaultConfiguration::registerEntries(const std::vector<Configuration::EntryData>& entries)
{
    for(const auto& entry: entries)
        addEntry(entry);
}

