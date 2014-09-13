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

#include <OpenLibrary/utils/optional.hpp>

#include "configuration_export.h"

class QString;

namespace Configuration
{
    class EntryData;
    class ConfigurationKey;

    //Base configuration keys
    extern CONFIGURATION_EXPORT const char* configLocation;       //application dir with configuration,
    //databases etc
}

struct IConfiguration
{
    IConfiguration() {}
    virtual ~IConfiguration() {}

    // This function registers a default entry with value.
    // If a value for specified entry exists in config file,
    // will be prefered.
    virtual void registerDefaultEntries(const std::vector<Configuration::EntryData> &) = 0;

    // This function introduces a configuration key.
    // Before adding/loading values to configuration, each entry (Key) must be introduced.
    // Unknown (not registered) keys will be treated as deprecated,
    // and dev-warning will be printed in output.
    virtual void registerKey(const Configuration::ConfigurationKey &) = 0;

    //function registers configuration key names and defaults from XML string
    virtual void registerXml(const QString &) = 0;

    // Add entry to config
    virtual void addEntry(const Configuration::EntryData &) = 0;

    // loads data from initializers and disk storage
    virtual bool load() = 0;

    virtual Optional<Configuration::EntryData> findEntry(const Configuration::ConfigurationKey &) const = 0;
    virtual const std::vector<Configuration::EntryData> getEntries() = 0;
};

#endif  //ICONFIGURATION_HPP
