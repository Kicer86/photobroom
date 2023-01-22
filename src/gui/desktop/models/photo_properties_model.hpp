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

#include <QStandardItemModel>

#include <database/idatabase.hpp>
#include <database/photo_data.hpp>
#include <database/database_tools/id_to_data_converter.hpp>


class PhotoPropertiesModel: public QStandardItemModel
{
        Q_OBJECT
        Q_PROPERTY(Database::IDatabase* database WRITE setDatabase READ database REQUIRED)
        Q_PROPERTY(bool busy READ busy NOTIFY busyChanged)

    public:
        explicit PhotoPropertiesModel(QObject * = nullptr);
        ~PhotoPropertiesModel();

        void setDatabase(Database::IDatabase *);
        Q_INVOKABLE void setPhotos(const std::vector<Photo::Id> &);

        Database::IDatabase* database() const;
        bool busy() const;

    private:
        Database::IDatabase* m_db = nullptr;
        std::unique_ptr<IdToDataConverter> m_translator;
        bool m_busy = false;

        void gotPhotoData(const std::vector<Photo::DataDelta> &);
        void refreshLabels(const std::vector<Photo::DataDelta> &);
        void refreshValues(const std::vector<Photo::DataDelta> &);

        QString sizeHuman(qint64) const;
        void setBusy(bool);

    signals:
        void busyChanged(bool) const;
};

#endif // PHOTOPROPERTIES_HPP
