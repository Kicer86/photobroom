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
#include <unordered_map>
#include <unordered_set>

#include <QString>
#include <QXmlStreamReader>
#include <QFile>

#include "system/system.hpp"

#include "entrydata.hpp"


namespace
{

    struct hash
    {
        std::size_t operator()(const Configuration::ConfigurationKey& s) const
        {
            return std::hash<std::string>()(s.getKeyRaw());
        }
    };
}

struct DefaultConfiguration::Impl
{
    Impl(): m_known_keys(), m_data() {}

    std::string getConfigDir() const
    {
        const std::string result = System::getApplicationConfigDir();

        return result;
    }

    boost::optional<Configuration::EntryData> find(const Configuration::ConfigurationKey& key) const
    {
        verifyKey(key);

        boost::optional<Configuration::EntryData> result;
        auto it = m_data.find(key);

        if (it != m_data.end())
            result = it->second;

        return result;
    }

    std::vector<Configuration::EntryData> getAll() const
    {
        std::vector<Configuration::EntryData> result;
        for(const auto& it: m_data)
            result.push_back(it.second);

        return result;
    }

    void addEntry(const Configuration::ConfigurationKey& key, const Configuration::EntryData& data, bool def = false)
    {
        verifyKey(key);

        //for default entries
        //add only those which are not present yet
        bool add = def? m_data.find(key) == m_data.end(): true;

        if (add)
            m_data[key] = data;
    }

    void introduceKey(const Configuration::ConfigurationKey& key)
    {
        m_known_keys.insert(key);
    }

    bool loadXml(const QString& path)
    {
        QFile data(path);
        bool status = data.open(QIODevice::ReadOnly);

        if (status)
        {
            QString buffer = data.readAll();

            status = useXml(buffer);
        }

        return status;
    }

    bool useXml(const QString &xml)
    {
        QXmlStreamReader reader(xml);
        int level = 0;
        bool status = true;

        while(status && reader.atEnd() == false)
        {
            if (reader.isStartElement())
            {
                std::cout << "DefaultConfiguration: found start element: " << reader.name().toString().toStdString() << std::endl;
                level++;

                const QStringRef name = reader.name();

                if (level == 1 && name == "configuration")
                {
                    //just do nothing
                }
                else if (level == 2 && name == "keys")
                {
                    status = parseXml_Keys(&reader);
                    gotoNextUseful(&reader);            //jump over end element
                }
                else if (level == 2 && name == "defaults")
                {
                    status = parseXml_DefaultKeys(&reader);
                    gotoNextUseful(&reader);            //jump over end element
                }
                else
                {
                    std::cerr << "DefaultConfiguration: invalid format of xml file (unknown tag: "
                                << name.toString().toStdString()
                                << ")"
                                << std::endl;

                    status = false;
                    break;
                }
            }

            if (reader.isEndElement())
            {
                std::cout << "DefaultConfiguration: found end element: " << reader.name().toString().toStdString() << std::endl;
                level--;
            }

            reader.readNext();
        }

        return status;
    }

    private:
        std::unordered_set<Configuration::ConfigurationKey, hash> m_known_keys;
        std::unordered_map<Configuration::ConfigurationKey, Configuration::EntryData, hash> m_data;

        bool parseXml_Keys(QXmlStreamReader* reader)
        {
            bool status = true;

            status = gotoNextUseful(reader);

            while (status && reader->tokenType() == QXmlStreamReader::StartElement)       //start element should came
            {
                const QStringRef token = reader->name();

                if (token == "key")
                {
                    const QXmlStreamAttributes attrs = reader->attributes();
                    const QStringRef name = attrs.value("name");

                    if (name.isEmpty() == false)
                    {
                        Configuration::ConfigurationKey key(name.toString());
                        introduceKey(key);
                    }
                    else
                        std::cerr << "DefaultConfiguration: in <keys> section there is a key with null name" << std::endl;
                }
                else
                {
                    status = false;
                    break;
                }

                status = gotoNextUseful(reader);
                if (status == false)
                    break;

                const QXmlStreamReader::TokenType type = reader->tokenType();   //now we expect end element

                if (type != QXmlStreamReader::EndElement)
                {
                    status = false;
                    break;
                }
            }

            //here we expect end element for <keys>
            const QXmlStreamReader::TokenType type = reader->tokenType();
            if (type != QXmlStreamReader::EndElement)
                status = false;

            return status;
        }

        bool parseXml_DefaultKeys(QXmlStreamReader* reader)
        {
            bool status = true;

            status = gotoNextUseful(reader);

            while (status && reader->tokenType() == QXmlStreamReader::StartElement)       //start element should came
            {
                const QStringRef token = reader->name();

                if (token == "key")
                {
                    const QXmlStreamAttributes attrs = reader->attributes();
                    const QStringRef name = attrs.value("name");
                    const QStringRef value = attrs.value("value");

                    if (name.isEmpty() == false)
                    {
                        //introduce key
                        Configuration::ConfigurationKey key(name.toString());
                        introduceKey(key);

                        //set default value
                        Configuration::EntryData entry(key, value.toString().toStdString());
                        addEntry(key, entry);
                    }
                    else
                        std::cerr << "DefaultConfiguration: in <defaults> section there is a key with null name" << std::endl;
                }
                else
                {
                    status = false;
                    break;
                }

                status = gotoNextUseful(reader);
                if (status == false)
                    break;

                const QXmlStreamReader::TokenType type = reader->tokenType();   //now we expect end element

                if (type != QXmlStreamReader::EndElement)
                {
                    status = false;
                    break;
                }
            }

            //here we expect end element for <keys>
            const QXmlStreamReader::TokenType type = reader->tokenType();
            if (type != QXmlStreamReader::EndElement)
                status = false;

            return status;
        }

        bool gotoNextUseful(QXmlStreamReader* reader)
        {
            QXmlStreamReader::TokenType type = QXmlStreamReader::Invalid;

            do
            {
                type = reader->readNext();
            }
            while(type == QXmlStreamReader::Comment || type == QXmlStreamReader::Characters);

            const bool status = reader->hasError() == false &&
                                type != QXmlStreamReader::Invalid &&
                                type != QXmlStreamReader::NoToken;

            return status;
        }

        void verifyKey(const Configuration::ConfigurationKey& key) const
        {
            if (m_known_keys.find(key) == m_known_keys.end())
                std::cerr << "DefaultConfiguration: unknown key: " << key.getKeyRaw() << std::endl;
        }
};


DefaultConfiguration::DefaultConfiguration(): m_impl(new Impl)
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

