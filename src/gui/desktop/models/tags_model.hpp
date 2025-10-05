/*
 * Photo Broom - photos management tool.
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

#include <QAbstractItemModel>

#include <database/idatabase.hpp>
#include "gui/desktop/utils/photo_delta_fetcher_binding.hpp"

#include "tags_operator.hpp"


class TagsModel: public QAbstractItemModel
{
        Q_OBJECT
        Q_PROPERTY(Database::IDatabase* database WRITE set READ getDatabase REQUIRED)
        Q_PROPERTY(bool busy READ busy NOTIFY busyChanged)

    public:
        enum Roles
        {
            TagTypeRole = Qt::UserRole,
        };

        explicit TagsModel(QObject * = 0);
        TagsModel(const TagsModel &) = delete;
        ~TagsModel();

        void set(Database::IDatabase *);

        Q_INVOKABLE void setPhotos(const std::vector<Photo::Id> &);

        TagsModel& operator=(const TagsModel &) = delete;

        Tag::TagsList getTags() const;
        Database::IDatabase* getDatabase() const;
        bool busy() const;

        // overrides:
        bool setData(const QModelIndex & index, const QVariant & value, int role) override;
        bool setItemData(const QModelIndex & index, const QMap<int, QVariant> & roles) override;
        bool insertRows(int row, int count, const QModelIndex & parent) override;
        QHash<int, QByteArray> roleNames() const override;

        QVariant data(const QModelIndex & index, int role) const override;
        Qt::ItemFlags flags(const QModelIndex &index) const override;
        QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const override;
        QModelIndex parent(const QModelIndex & child) const override;
        int columnCount(const QModelIndex & parent = QModelIndex()) const override;
        int rowCount(const QModelIndex & parent = QModelIndex()) const override;
        QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    signals:
        void busyChanged(bool) const;

    private:
        typedef QMap<int, QVariant> ItemData;
        std::vector<ItemData> m_keys,
                              m_values;
        TagsOperator m_tagsOperator;
        Gui::Utils::PhotoDeltaFetcherBinding<TagsModel, void (TagsModel::*)(const std::vector<Photo::DataDelta>&)> m_translator;
        Database::IDatabase* m_database;
        bool m_busy = false;

        void clearModel();
        void loadPhotos(const std::vector<Photo::DataDelta> &);
        void syncData(const QModelIndex &, const QModelIndex &);
        QVector<int> setDataInternal(const QModelIndex & index, const QVariant & value, int role);
        QVariant correctInput(const QModelIndex &, const QVariant &) const;

        void setBusy(bool);
};

#endif // TAGSMODEL_HPP
