/*
 * private part of configuration class
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

#ifndef CONFIGURATIONPRIVATE_HPP
#define CONFIGURATIONPRIVATE_HPP


#include <QJsonObject>

#include <OpenLibrary/putils/ts_resource.hpp>


class ConfigurationPrivate
{
    public:
        ConfigurationPrivate(Configuration* q);
        virtual ~ConfigurationPrivate();

        void ensureDataLoaded();

    private:
        ol::ThreadSafeResource<QJsonObject> m_json;
        class Configuration* const q;
        bool m_loaded;

        void loadData();
};

#endif // CONFIGURATIONPRIVATE_HPP
