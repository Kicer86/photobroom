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

#include <functional>
#include <set>

#include <QTimer>

#include <json/value.h>

#include <OpenLibrary/putils/ts_resource.hpp>


struct IConfigObserver;

class ConfigurationPrivate: public QObject
{
    public:
        ConfigurationPrivate();
        ConfigurationPrivate(const ConfigurationPrivate &) = delete;
        virtual ~ConfigurationPrivate();

        ConfigurationPrivate& operator=(const ConfigurationPrivate &) = delete;

        QVariant getEntry(const QString &);
        void setEntry(const QString &, const QVariant &);

        void registerObserver(IConfigObserver *);
        void watchFor(const QString &, const IConfiguration::Watcher &);

    private:
        ol::ThreadSafeResource<Json::Value> m_json;
        QTimer m_dumpTimer;
        std::set<IConfigObserver *> m_observers;
        std::map<QString, std::vector<IConfiguration::Watcher>> m_watchers;

        void loadData();
        void markDataDirty();
        void saveData();

        void solve(const QString &, std::function<void(Json::Value &)>);
};

#endif // CONFIGURATIONPRIVATE_HPP
