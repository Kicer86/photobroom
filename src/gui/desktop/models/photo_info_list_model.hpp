
/*
 * Plain list of IPHotoInfo
 * Copyright (C) 2016  Michał Walenciak <Kicer86@gmail.com>
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

#ifndef PHOTOINFOLISTMODEL_HPP
#define PHOTOINFOLISTMODEL_HPP

#include "aphoto_info_model.hpp"


// TODO: unit test me!
class PhotoInfoListModel: public APhotoInfoModel
{
    public:
        PhotoInfoListModel(QObject * = 0);
        PhotoInfoListModel(const PhotoInfoListModel &) = delete;
        virtual ~PhotoInfoListModel();

        PhotoInfoListModel& operator=(const PhotoInfoListModel &) = delete;

        void set(const std::vector<IPhotoInfo::Ptr> &);

        // QAbstractItemModel overrides:
        virtual int columnCount(const QModelIndex& parent = QModelIndex()) const override;
        virtual QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
        virtual QModelIndex parent(const QModelIndex& child) const override;
        virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    protected:
        // APhotoInfoModel overrides:
        virtual QVariant data(const QModelIndex& _index, int role) const override;
        virtual IPhotoInfo* getPhotoInfo(const QModelIndex &) const override;

    private:
        std::vector<IPhotoInfo::Ptr> m_photos;
};

#endif // PHOTOINFOLISTMODEL_HPP
