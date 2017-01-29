/*
 * class used for finding right place in tree for new photos
 * Copyright (C) 2014  Michał Walenciak <email>
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

#ifndef PHOTOSMATCHER_HPP
#define PHOTOSMATCHER_HPP

#include <database/iphoto_info.hpp>
#include <database/filter.hpp>

class IdxDataManager;
class DBDataModel;
struct IIdxData;

class PhotosMatcher
{
    public:
        PhotosMatcher();
        PhotosMatcher(const PhotosMatcher &) = delete;
        ~PhotosMatcher();

        void set(IdxDataManager *);
        void set(DBDataModel *);

        PhotosMatcher& operator=(const PhotosMatcher &) = delete;

        bool doesMatchModelFilters(const IPhotoInfo::Ptr &) const;
        bool doesMatchFilter(const IPhotoInfo::Ptr &, const Database::IFilter::Ptr &);
        IIdxData* findParentFor(const IPhotoInfo::Ptr &) const;
        IIdxData* findCloserAncestorFor(const IPhotoInfo::Ptr &) const;

    private:
        IdxDataManager* m_idxDataManager;
        DBDataModel* m_dbDataModel;

        IIdxData* findParentFor(const IPhotoInfo::Ptr &, bool) const;
};

#endif // PHOTOSMATCHER_HPP
