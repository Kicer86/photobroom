/*
 * Photo Broom - photos management tool.
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

#include "ts_resource.hpp"

class QJsonValueRef;
struct IConfigObserver;

class ConfigurationPrivate final: public QObject
{
    public:
        /**
         * \param configStorage object responsible for storing and restoring configuration
         */
        explicit ConfigurationPrivate(IConfigStorage& configStorage);
        ConfigurationPrivate(const ConfigurationPrivate &) = delete;
        virtual ~ConfigurationPrivate();

        ConfigurationPrivate& operator=(const ConfigurationPrivate &) = delete;

        QVariant getEntry(const QString &);
        void setEntry(const QString &, const QVariant &);

        void watchFor(const QString &, const IConfiguration::Watcher &);

    private:
        IConfigStorage& m_configStorage;
        ol::ThreadSafeResource<IConfigStorage::Content> m_entries;
        QTimer m_dumpTimer;
        std::map<QString, std::vector<IConfiguration::Watcher>> m_watchers;

        void loadData();
        void markDataDirty();
        void saveData();
};

#endif // CONFIGURATIONPRIVATE_HPP
