/*
 * Tags model
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

#ifndef TAGSMODEL_HPP
#define TAGSMODEL_HPP

#include <QStandardItemModel>

#include <database/iphoto_info.hpp>

class QItemSelectionModel;
class QItemSelection;

class DBDataModel;

class TagsModel: public QStandardItemModel
{
        Q_OBJECT

    public:
        TagsModel(QObject * = 0);
        TagsModel(const TagsModel &) = delete;
        ~TagsModel();

        void set(QItemSelectionModel *);    // selection model
        void set(DBDataModel *);            // photos model

        TagsModel& operator=(const TagsModel &) = delete;

    private:
        QItemSelectionModel* m_selectionModel;
        DBDataModel* m_dbDataModel;

        void refreshModel();
        void clearModel();
        std::vector<IPhotoInfo::Ptr> getPhotosForSelection();

    private slots:
        void refreshModel(const QItemSelection &, const QItemSelection &);
};

#endif // TAGSMODEL_HPP
