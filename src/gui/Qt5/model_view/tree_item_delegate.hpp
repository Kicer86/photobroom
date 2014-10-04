/*
 * Default delegate for ImageTreeView
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

#ifndef TREEITEMDELEGATE_HPP
#define TREEITEMDELEGATE_HPP

#include <memory>

#include <QAbstractItemDelegate>

class TreeItemDelegate: public QAbstractItemDelegate
{
    public:
        TreeItemDelegate();
        TreeItemDelegate(const TreeItemDelegate &) = delete;
        ~TreeItemDelegate();
        TreeItemDelegate& operator=(const TreeItemDelegate &) = delete;

        virtual QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const;
        virtual void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;

    private:
        struct RotationData
        {
            int rotation;
        };
        std::unique_ptr<QCache<QModelIndex, RotationData>> m_rotationData;

        void paintImage(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
        void paintNode(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
        
        int getRotationFor(const QModelIndex &) const;
};

#endif // TREEITEMDELEGATE_HPP
