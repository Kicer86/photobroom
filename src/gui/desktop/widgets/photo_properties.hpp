/*
 * Photo Broom - photos management tool.
 * Copyright (C) 2017  Michał Walenciak <Kicer86@gmail.com>
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

#ifndef PHOTOPROPERTIES_HPP
#define PHOTOPROPERTIES_HPP

#include <QScrollArea>
#include <QStandardItemModel>

#include <database/photo_data.hpp>


class APhotoInfoModel;


class PhotoPropertiesModel: public QStandardItemModel
{
        Q_OBJECT

    public:
        PhotoPropertiesModel(QObject * = nullptr);
        ~PhotoPropertiesModel();

        void setPhotos(const std::vector<Photo::Data> &);

    private:
        void refreshLabels(const std::vector<Photo::Data> &);
        void refreshValues(const std::vector<Photo::Data> &);

        QString sizeHuman(qint64) const;
};

#endif // PHOTOPROPERTIES_HPP
