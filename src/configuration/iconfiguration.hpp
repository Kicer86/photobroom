/*
    Configuration interface
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

#ifndef ICONFIGURATION_HPP
#define ICONFIGURATION_HPP

#include <vector>
#include <string>

#include <boost/optional.hpp>

namespace Configuration
{    
    class EntryData;
    class ConfigurationKey;
    
    //Base configuration keys
    extern const std::string configLocation;
}

struct IConfiguration
{
    IConfiguration() {}
    virtual ~IConfiguration() {}

    virtual void addEntry(const Configuration::EntryData &) = 0;
    virtual void registerEntries(const std::vector<Configuration::EntryData> &) = 0;
    
    virtual boost::optional<Configuration::EntryData> findEntry(const Configuration::ConfigurationKey &) const = 0;    
    virtual const std::vector<Configuration::EntryData> getEntries() = 0;
};

#endif  //ICONFIGURATION_HPP
