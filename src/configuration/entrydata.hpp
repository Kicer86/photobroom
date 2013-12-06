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


#ifndef ENTRYDATA_HPP
#define ENTRYDATA_HPP

#include <vector>
#include <string>

#include <OpenLibrary/utils/data_ptr.hpp>

class ConfigurationKey
{
    public:
        ConfigurationKey();
        ConfigurationKey(const std::string &);
        ConfigurationKey(const char *);
        virtual ~ConfigurationKey();
        
        std::vector<std::string> getKey() const;
        std::string getKeyRaw() const;
        
        void setKey(const std::string &);
        void setKey(const std::vector<std::string> &);
        
        bool operator==(const ConfigurationKey& other) const;
        
    private:
        struct Data;
        data_ptr<Data> m_data;
};

class EntryData
{
    public:
        EntryData();
        EntryData(const ConfigurationKey &, const std::string &);
        virtual ~EntryData();
        virtual bool operator==(const EntryData& other) const;

    private:
        struct Data;
        data_ptr<Data> m_data;
};

#endif // ENTRYDATA_HPP
