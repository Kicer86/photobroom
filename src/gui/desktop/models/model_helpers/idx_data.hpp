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

#include "models/db_data_model.hpp"
#include "models/model_types.hpp"

class QVariant;

class IdxDataManager;

class IdxData
{
    public:
        std::vector<IdxData *> m_children;
        QMap<int, QVariant> m_data;
        Database::IFilter::Ptr m_filter;         // define which children match
        Hierarchy::Level m_order;                // defines how to sort children
        IPhotoInfo::Ptr m_photo;                 // null for nodes, photo for photos
        IdxDataManager* m_model;
        size_t m_level;

        // node constructor
        IdxData(IdxDataManager *, const QVariant& name);

        //leaf constructor
        IdxData(IdxDataManager *, const IPhotoInfo::Ptr &);

        virtual ~IdxData();

        IdxData(const IdxData &) = delete;
        IdxData& operator=(const IdxData &) = delete;

        void setNodeFilter(const Database::IFilter::Ptr& filter);
        void setNodeSorting(const Hierarchy::Level &);
        long findPositionFor(const IdxData* child) const;     // returns position where child matches
        long getPositionOf(const IdxData* child) const;       // returns position of children
        void addChild(IdxData* child);
        void removeChild(IdxData* child);                     // removes child (memory is released)
        void takeChild(IdxData* child);                       // function acts as removeChild but does not delete children
        void reset();
        void setParent(IdxData *);
        void setStatus(NodeStatus);
        IdxData* parent() const;
        bool isPhoto() const;
        bool isNode() const;

        int getRow() const;
        int getCol() const;

        NodeStatus status() const;

        IdxData* findChildWithBadPosition() const;            // returns first child which lies in a wrong place
        bool sortingRequired() const;

    private:
        IdxData* m_parent;

        IdxData(IdxDataManager *);
        void initLeafData();
        void init();
};

#endif // IDXDATA_HPP
