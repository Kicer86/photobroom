/*
 * Tree View for Images.
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

#ifndef IMAGESTREEVIEW_H
#define IMAGESTREEVIEW_H

#include <QAbstractItemView>

#include <memory>
#include <deque>

struct IConfiguration;
class ImagesTreeView: public QAbstractItemView
{
        Q_OBJECT

    public:
        ImagesTreeView(QWidget *);
        ImagesTreeView(const ImagesTreeView &) = delete;
        ~ImagesTreeView();

        ImagesTreeView& operator=(const ImagesTreeView &) = delete;

        void set(IConfiguration *);

        // QAbstractItemView overrides:
        virtual QModelIndex indexAt(const QPoint& point) const override;
        virtual bool isIndexHidden(const QModelIndex& index) const override;
        virtual QRect visualRect(const QModelIndex& index) const override;
        virtual QRegion visualRegionForSelection(const QItemSelection& selection) const override;
        virtual int horizontalOffset() const override;
        virtual int verticalOffset() const override;

        virtual QModelIndex moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers) override;
        virtual void scrollTo(const QModelIndex& index, ScrollHint hint = EnsureVisible) override;
        virtual void setSelection(const QRect& rect, QItemSelectionModel::SelectionFlags command) override;

        virtual void setModel(QAbstractItemModel *) override;

        // QWidget overrides:
        virtual void paintEvent(QPaintEvent*) override;
        virtual void mouseReleaseEvent(QMouseEvent*) override;

    private:
        struct Data;
        std::unique_ptr<Data> m_data;

        // positions calculations
        QRect calcItemRect(const QModelIndex &) const;
        QPoint positionOfNext(const QModelIndex &) const;                             // calculate position of next item
        QPoint positionOfNextImage(const QModelIndex &) const;                        // calculate position of next item (image item)
        QPoint positionOfNextNode(const QModelIndex &) const;                         // calculate position of next item (node item)
        QPoint positionOfFirstChild(const QModelIndex &) const;                       // calculate position for first item
        QPoint matrixPositionOf(const QModelIndex &) const;                           // get position (in matrix) of item
        QModelIndex itemAtMatrixPosition(const QPoint &, QModelIndex& parent) const;  // convert position (in matrix) to item
        int itemsPerRow() const;
        bool isImage(const QModelIndex &) const;
        QPixmap getImage(const QModelIndex &) const;
        int getitemWidth(const QModelIndex &) const;
        int getItemHeigth(const QModelIndex &) const;
        int getItemHeigth(const QModelIndex &, const QModelIndex &) const;                // max height for set of items
        QSize getItemSize(const QModelIndex &) const;

        // view stuff
        QRect getItemRect(const QModelIndex &) const;
        std::deque<QModelIndex> findItemsIn(const QRect &) const;
        std::deque<QModelIndex> getChildrenFor(const QModelIndex &) const;
        bool isExpanded(const QModelIndex &) const;

        // model updates
        void rereadModel();

    private slots:
        void modelReset();
        void rowsInserted(const QModelIndex &, int, int);
};

#endif // IMAGESTREEVIEW_H
