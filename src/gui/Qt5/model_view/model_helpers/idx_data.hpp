/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2014  Michał Walenciak <MichalWalenciak@gmail.com>
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

#ifndef IDXDATA_HPP
#define IDXDATA_HPP

#include <QMap>

#include <database/iphoto_info.hpp>
#include <database/filter.hpp>

class QVariant;

class IdxDataManager;

class IdxData: public IPhotoInfo::IObserver
{
	public:
        enum class FetchStatus
        {
            NotFetched,
            Fetching,
            Fetched,
        };

        std::vector<IdxData *> m_children;
        QMap<int, QVariant> m_data;
        Database::IFilter::Ptr m_filter;
        IPhotoInfo::Ptr m_photo;                 // null for nodes, photo for photos
        IdxData* m_parent;
        IdxDataManager* m_model;
        size_t m_level;
        int m_row;
        int m_column;
        FetchStatus m_loaded;                    // true when we have loaded all children of item (if any)

        // node constructor
        IdxData(IdxDataManager *, IdxData* parent, const QString& name);

        //leaf constructor
        IdxData(IdxDataManager *, IdxData* parent, const IPhotoInfo::Ptr &);

        virtual ~IdxData();

        IdxData(const IdxData &) = delete;
        IdxData& operator=(const IdxData &) = delete;

        void setNodeData(const Database::IFilter::Ptr& filter);
        void addChild(IdxData* child);
        void removeChild(IdxData* child);
        void reset();
        bool isPhoto() const;
        bool isNode() const;

    private:
        IdxData(IdxDataManager *, IdxData* parent);
        void setPosition(int row, int col);
        void updateLeafData();
        void init();

        //IObserver:
        void photoUpdated(IPhotoInfo *) override;
};

#endif // IDXDATA_HPP
