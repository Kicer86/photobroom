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

#ifndef IMAGESTREEVIEW_HPP
#define IMAGESTREEVIEW_HPP

#include <QAbstractItemView>

#include <memory>
#include <vector>

#include <utils/signal_filter.hpp>

class QTimer;

struct IConfiguration;
class Data;


class ImagesTreeView: public QAbstractItemView
{
        Q_OBJECT

    public:
        ImagesTreeView(QWidget * = nullptr);
        ImagesTreeView(const ImagesTreeView &) = delete;
        ~ImagesTreeView();

        ImagesTreeView& operator=(const ImagesTreeView &) = delete;

        void setSpacing(int);
        void setImageMargin(int);
        void setThumbnailHeight(int);
        void invalidate();                // call it after above methods

        int getThumbnailHeight() const;
        QRect childrenSize(const QModelIndex &) const;

        // QAbstractItemView overrides:
        virtual QModelIndex indexAt(const QPoint& point) const override;
        virtual bool isIndexHidden(const QModelIndex& index) const override;
        virtual QRect visualRect(const QModelIndex& index) const override;
        virtual QRegion visualRegionForSelection(const QItemSelection& selection) const override;
        virtual int horizontalOffset() const override;
        virtual int verticalOffset() const override;

        virtual QModelIndex moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers) override;
        virtual void scrollContentsBy(int dx, int dy) override;
        virtual void scrollTo(const QModelIndex& index, ScrollHint hint = EnsureVisible) override;
        virtual void setSelection(const QRect& rect, QItemSelectionModel::SelectionFlags command) override;

        virtual void setModel(QAbstractItemModel *) override;

    protected:
        // QWidget overrides:
        virtual void paintEvent(QPaintEvent *) override;
        virtual void mouseMoveEvent(QMouseEvent *) override;
        virtual void mousePressEvent(QMouseEvent *) override;
        virtual void mouseReleaseEvent(QMouseEvent *) override;
        virtual void resizeEvent(QResizeEvent *) override;

    private:
        std::unique_ptr<Data> m_data;
        SignalFilter m_viewStatus;
        QModelIndex m_previouslySelectedItem;
        QPoint m_regionSelectionStartPoint;
        bool m_dataDirty;
        bool m_regionSelectionActive;

        // view stuff
        const QRect getItemRect(const QModelIndex &) const;
        std::vector<QModelIndex> findItemsIn(const QRect &);

        void setSelection(const QModelIndex &, const QModelIndex &, QItemSelectionModel::SelectionFlags);

        // widget operations
        void updateData();
        void updateGui();
        QPoint getOffset() const;

    private slots:
        void dataChanged(const QModelIndex &, const QModelIndex &, const QVector<int> &) override;
        void modelReset();
        void rowsInserted(const QModelIndex &, int, int) override;
        void rowsMoved(const QModelIndex &, int, int, const QModelIndex &, int);
        void rowsRemoved(const QModelIndex &, int, int);

        // model updates
        void updateView();

    signals:
        void refreshView();
        void contentScrolled();
};

#endif // IMAGESTREEVIEW_HPP
