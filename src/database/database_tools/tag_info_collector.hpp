/*
 * Database observer, tags analyzer and collector
 * Copyright (C) 2016  Michał Walenciak <MichalWalenciak@gmail.com>
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

#ifndef TAGINFOCOLLECTOR_HPP
#define TAGINFOCOLLECTOR_HPP

#include <mutex>

#include "itag_info_collector.hpp"
#include "../iphoto_info.hpp"
#include "database_export.h"

namespace Database
{
    struct IDatabase;
}

class DATABASE_EXPORT TagInfoCollector: public QObject, public ITagInfoCollector
{
    public:
        TagInfoCollector();
        TagInfoCollector(const TagInfoCollector &) = delete;
        ~TagInfoCollector();

        TagInfoCollector& operator=(const TagInfoCollector &) = delete;

        void set(Database::IDatabase *);

        const std::vector<TagValue>& get(const TagNameInfo &) const override;
        int registerChangeObserver( const std::function< void(const TagNameInfo &) > & ) override;
        void unregisterChangeObserver(int) override;

    private:
        mutable std::map<TagNameInfo, std::vector<TagValue>> m_tags;
        mutable std::mutex m_tags_mutex;
        std::map< int, std::function<void(const TagNameInfo &)> > m_observers;
        Database::IDatabase* m_database;
        int m_observerId;

        void gotTagValues(const TagNameInfo &, const std::vector<TagValue> &);
        void photoModified(const IPhotoInfo::Ptr &);

        void updateAllTags();
        void updateValuesFor(const TagNameInfo &);

        void notifyObserversAbout(const TagNameInfo &) const;
};

#endif // TAGINFOCOLLECTOR_H
