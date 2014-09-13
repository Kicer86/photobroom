/*
    Configuration entry
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


#include "entrydata.hpp"

#include <cstring>
#include <string>

#include <QString>

namespace Configuration
{

    struct ConfigurationKey::Data
    {
            Data(): m_key()
            {

            }

            Data(const std::string& key): m_key(key)
            {
            }

            Data(const QString& key): m_key(key.toStdString())
            {
            }

            void setKey(const std::string& raw)
            {
                m_key = raw;
            }

            void setKey(const std::vector<std::string>& key)
            {
                m_key = convert(key);
            }

            std::string getKeyRaw() const
            {
                return m_key;
            }

            std::vector<std::string> getKey() const
            {
                return convert(m_key);
            }

            static std::vector<std::string> convert(const std::string& raw)
            {
                char *rawData = new char[raw.size() + 1];
                strcpy(rawData, raw.c_str());

                std::vector<char *> pointers;
                size_t first_pos = 0, i = 0;

                while (i <= raw.size())
                {
                    if (rawData[i] == ':' || rawData[i] == '\0')     // ':' means that '::' goes
                    {
                        rawData[i++] = '\0';
                        i++;                     // jump over first and second ':'

                        pointers.push_back(&rawData[first_pos]);  //save current string
                        first_pos = i;                            //mark first char of next one
                    }
                    else
                        i++;
                }

                std::vector<std::string> result(pointers.size());

                for (size_t j = 0; j < pointers.size(); j++)
                    result[j] = pointers[j];

                delete [] rawData;

                return result;
            }

            static std::string convert(const std::vector<std::string>& key)
            {
                std::string result;

                for(size_t i = 0; i < key.size(); i++)
                {
                    result += key[i];

                    if (i + 1 < key.size())
                        result += "::";
                }

                return result;
            }

        private:
            std::string m_key;
    };


    ConfigurationKey::ConfigurationKey(): m_data(new Data)
    {

    }


    ConfigurationKey::ConfigurationKey(const std::string& key): m_data(new Data(key))
    {

    }


    ConfigurationKey::ConfigurationKey(const char *key): m_data(new Data(std::string(key)))
    {

    }


    ConfigurationKey::ConfigurationKey(const QString& key): m_data(new Data(key))
    {

    }


    ConfigurationKey::~ConfigurationKey()
    {

    }


    std::vector< std::string > ConfigurationKey::getKey() const
    {
        return m_data->getKey();
    }


    std::string ConfigurationKey::getKeyRaw() const
    {
        return m_data->getKeyRaw();
    }


    void ConfigurationKey::setKey(const std::string& key)
    {
        m_data->setKey(key);
    }


    void ConfigurationKey::setKey(const std::vector<std::string>& key)
    {
        m_data->setKey(key);
    }


    bool ConfigurationKey::operator==(const ConfigurationKey& other) const
    {
        return m_data->getKey() == other.m_data->getKey();
    }

    /************************************************************************/

    struct EntryData::Data
    {
        Data(): m_key(), m_value() {}

        Data(const ConfigurationKey& key, const QString& value): m_key(key), m_value(value)
        {

        }

        virtual ~Data() {}

        ConfigurationKey m_key;
        QString m_value;
    };


    EntryData::EntryData(): m_data(new Data)
    {

    }


    EntryData::EntryData(const ConfigurationKey& k, const QString& v): m_data(new Data(k, v))
    {

    }


    EntryData::~EntryData()
    {

    }


    bool EntryData::operator==(const EntryData& other) const
    {
        const bool status = m_data->m_key == other.m_data->m_key &&
                            m_data->m_value == other.m_data->m_value;

        return status;
    }


    QString EntryData::value() const
    {
        return m_data->m_value;
    }


    Configuration::ConfigurationKey EntryData::key() const
    {
        return m_data->m_key;
    }

}
