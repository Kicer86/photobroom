/*
 * Base for models providing PhotoInfo.
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

#ifndef ASCALABLEIMAGESMODEL_HPP
#define ASCALABLEIMAGESMODEL_HPP

#include <QAbstractItemModel>
#include <database/iphoto_info.hpp>


struct ITaskExecutor;

class APhotoInfoModel: public QAbstractItemModel
{
    public:
        enum Roles
        {
            PhotoGeometry = Qt::UserRole,
            PhotoPath = Qt::UserRole + 1,
            LastRole = PhotoPath
        };

        struct PhotoDetails
        {
            Photo::Id id;
            QSize size;
            QString path;
        };

        APhotoInfoModel(QObject * = 0);
        APhotoInfoModel(const APhotoInfoModel &) = delete;
        ~APhotoInfoModel();

        APhotoInfoModel& operator=(const APhotoInfoModel &) = delete;

        QVariant data(const QModelIndex &, int role = Qt::DisplayRole) const override;

    protected:
        virtual PhotoDetails getPhotoDetails(const QModelIndex &) const = 0;
};


#endif // ASCALABLEIMAGESMODEL_HPP
