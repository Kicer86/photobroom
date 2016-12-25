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

#include "utils/selection_extractor.hpp"

struct ITagsOperator;
class QItemSelectionModel;
class QItemSelection;

class DBDataModel;

class TagsModel: public QStandardItemModel
{
        Q_OBJECT

    public:
        enum Roles
        {
            TagInfoRole = Qt::UserRole,
        };

        TagsModel(QObject * = 0);
        TagsModel(const TagsModel &) = delete;
        ~TagsModel();

        void set(QItemSelectionModel *);    // selection model
        void set(DBDataModel *);            // photos model
        void set(ITagsOperator *);

        TagsModel& operator=(const TagsModel &) = delete;

        Tag::TagsList getTags() const;
        void addTag(const TagNameInfo &, const TagValue &);

    private:
        SelectionExtractor m_selectionExtractor;
        QItemSelectionModel* m_selectionModel;
        DBDataModel* m_dbDataModel;
        ITagsOperator* m_tagsOperator;

        void refreshModel();
        void clearModel();

    private slots:
        void refreshModel(const QItemSelection &, const QItemSelection &);
        void updateData(const QModelIndex &, const QModelIndex &);

    signals:
        void modelChanged(bool);
};

#endif // TAGSMODEL_HPP
