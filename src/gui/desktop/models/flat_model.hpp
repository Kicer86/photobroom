/*
 * Copyright (C) 2020  Michał Walenciak <Kicer86@gmail.com>
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
 */

#ifndef FLATMODEL_HPP
#define FLATMODEL_HPP

#include "aphoto_info_model.hpp"

namespace Database
{
    struct IDatabase;
    struct IBackend;
}

class FlatModel : public QAbstractListModel
{
    public:
        FlatModel(QObject* = nullptr);

        enum Roles
        {
            PhotoIdRole = Qt::UserRole + 1,
            _lastRole = PhotoIdRole,
        };

        void setDatabase(Database::IDatabase *);

        QVariant data(const QModelIndex& index, int role) const override;
        int rowCount(const QModelIndex& parent) const override;
        QHash<int, QByteArray> roleNames() const override;

    private:
        std::vector<Photo::Id> m_photos;
        Database::IDatabase* m_db;

        void fetchedPhotos(const std::vector<Photo::Id> &);

        void fetchMatchingPhotos(Database::IBackend *);
};

#endif // FLATMODEL_HPP
