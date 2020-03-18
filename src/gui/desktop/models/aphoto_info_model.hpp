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

#ifndef ASCALABLEIMAGESMODEL_HPP
#define ASCALABLEIMAGESMODEL_HPP

#include <QAbstractItemModel>
#include <database/photo_data.hpp>


struct PhotoPropertiesObject
{
    Q_PROPERTY(QString path MEMBER m_path)
    Q_PROPERTY(int height MEMBER m_height)
    Q_PROPERTY(int width MEMBER m_width)

    PhotoPropertiesObject();
    PhotoPropertiesObject(const PhotoPropertiesObject &) = default;
    PhotoPropertiesObject(const QString& path, int h, int w);

    QString m_path;
    int m_height;
    int m_width;

    Q_GADGET
};


class APhotoInfoModel: public QAbstractItemModel
{
    public:
        enum Roles
        {
            PhotoProperties = Qt::UserRole + 1,
            _lastRole = PhotoProperties,
        };


        APhotoInfoModel(QObject * = nullptr);
        APhotoInfoModel(const APhotoInfoModel &) = delete;
        ~APhotoInfoModel();

        APhotoInfoModel& operator=(const APhotoInfoModel &) = delete;

        virtual const Photo::Data& getPhotoDetails(const QModelIndex &) const = 0;
        virtual QHash<int, QByteArray> roleNames() const override;

    protected:
        QHash<int, QByteArray> m_customRoles;

        void registerRole(int, const QByteArray &);
};


#endif // ASCALABLEIMAGESMODEL_HPP
