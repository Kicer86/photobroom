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

#include <QObject>
#include <QMap>

#include <core/aphoto_info.hpp>
#include <database/filter.hpp>

class DBDataModel;

struct IdxData: public QObject
{
    std::vector<IdxData *> m_children;
    QMap<int, QVariant> m_data;
    Database::FilterDescription m_filter;
    IPhotoInfo::Ptr m_photo;
    IdxData* m_parent;
    DBDataModel* m_model;
    size_t m_level;
    int m_row;
    int m_column;
    bool m_loaded;                          // true when we have loaded all children of item (if any)

    // node constructor
    IdxData(DBDataModel *, IdxData* parent, const QString& name);

    //leaf constructor
    IdxData(DBDataModel *, IdxData* parent, const IPhotoInfo::Ptr &);

    virtual ~IdxData();

    IdxData(const IdxData &) = delete;
    IdxData& operator=(const IdxData &) = delete;

    void setNodeData(const Database::FilterDescription& filter);
    void addChild(IdxData* child);
    void addChild(const APhotoInfo::Ptr& photoInfo);

    private:
        Q_OBJECT

        IdxData(DBDataModel *, IdxData* parent);
        void setPosition(int row, int col);
        void updateLeafData();

    private slots:
        void photoUpdated();
};

#endif // IDXDATA_HPP
