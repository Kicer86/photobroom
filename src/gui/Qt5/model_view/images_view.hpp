/*
    Images view for Model.
    Copyright (C) 2013  Michał Walenciak <MichalWalenciak@gmail.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/


#ifndef IMAGES_VIEW_HPP
#define IMAGES_VIEW_HPP

#include <memory>

#include <QAbstractItemView>

class PositionsCache;

class __attribute__((deprecated)) ImagesView: public QAbstractItemView
{
    public:
        explicit ImagesView(QWidget* p);
        virtual ~ImagesView();

        QModelIndexList getSelection() const;

        //QWidget's virtuals:
        virtual void paintEvent(QPaintEvent* );
        virtual void resizeEvent(QResizeEvent *e);

        //QAbstractItemView's pure virtuals:
        virtual QRect visualRect(const QModelIndex& index) const;
        virtual void scrollTo(const QModelIndex& idx, ScrollHint hint = EnsureVisible);
        virtual QModelIndex indexAt(const QPoint& point) const;
        virtual QModelIndex moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers);
        virtual int horizontalOffset() const;
        virtual int verticalOffset() const;
        virtual bool isIndexHidden(const QModelIndex& index) const;
        virtual void setSelection(const QRect& rect, QItemSelectionModel::SelectionFlags command);
        virtual QRegion visualRegionForSelection(const QItemSelection& selection) const;

        //QAbstractItemView's virtuals:
        virtual void dataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector< int >& roles = QVector<int>());
        virtual void rowsInserted(const QModelIndex& parent, int start, int end);
        virtual void rowsAboutToBeRemoved(const QModelIndex& parent, int start, int end);

    private:
        std::unique_ptr<PositionsCache> m_cache;
};

#endif // IMAGES_VIEW_HPP
