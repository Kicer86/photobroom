/*
 * A model proxy which adds extra row for data appending.
 * Copyright (C) 2018  Michał Walenciak <Kicer86@gmail.com>
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

#ifndef APPENDABLEMODELPROXY_HPP
#define APPENDABLEMODELPROXY_HPP

#include <QAbstractItemModel>


class AppendableModelProxy: public QAbstractItemModel
{
    public:
        AppendableModelProxy(QObject* = nullptr);
        ~AppendableModelProxy();

        //QAbstractItemModel overrides:
        int columnCount(const QModelIndex& parent) const override;
        int rowCount(const QModelIndex& parent) const override;
        QModelIndex parent(const QModelIndex& child) const override;
        QModelIndex index(int row, int column, const QModelIndex& parent) const override;
        Qt::ItemFlags flags(const QModelIndex &index) const override;
        QVariant data(const QModelIndex& index, int role) const override;
        QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
        bool setData(const QModelIndex & index, const QVariant & value, int role) override;

        void setSourceModel(QAbstractItemModel *sourceModel);

    private:
        typedef std::map<int, QVariant> CellData;
        std::vector<CellData> m_lastRowData;
        QAbstractItemModel* m_sourceModel;

        QModelIndex mapFromSource(const QModelIndex & sourceIndex) const;
        QModelIndex mapToSource(const QModelIndex & proxyIndex) const;

        void modelRowsAboutToBeInserted(const QModelIndex &, int, int);
        void modelRowsInserted(const QModelIndex &, int, int);
        void modelColumnsAboutToBeInserted(const QModelIndex &, int, int);
        void modelColumnsInserted(const QModelIndex &, int, int);
        void modelRowsAboutToBeRemoved(const QModelIndex &parent, int start, int end);
        void modelRowsRemoved(const QModelIndex &parent, int start, int end);
        void sourceModelAboutToBeReset();
        void sourceModelReset();
};

#endif // APPENDABLEMODELPROXY_H
