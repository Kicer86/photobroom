/*
 * Widget for Photos
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

#ifndef PHOTOSWIDGET_HPP
#define PHOTOSWIDGET_HPP

#include <QWidget>
#include <QTimer>

class QAbstractItemModel;
class QItemSelectionModel;
class QLineEdit;

class PhotosItemDelegate;
class DBDataModel;
class InfoBaloonWidget;
class ImagesTreeView;

struct IConfiguration;


class PhotosWidget: public QWidget
{
        Q_OBJECT

    public:
        PhotosWidget(QWidget * = nullptr);
        PhotosWidget(const PhotosWidget &) = delete;
        ~PhotosWidget();
        PhotosWidget& operator=(const PhotosWidget &) = delete;

        void set(IConfiguration *);
        void setModel(DBDataModel *);

        QItemSelectionModel* viewSelectionModel();

    private:
        QTimer m_timer;
        DBDataModel* m_model;
        ImagesTreeView* m_view;
        InfoBaloonWidget* m_info;
        PhotosItemDelegate* m_delegate;
        QLineEdit* m_searchExpression;

        virtual void changeEvent(QEvent *) override;

        void modelChanged(const QModelIndex &, int, int);
        void updateHint();
        void searchExpressionChanged(const QString &);
        void applySearchExpression();
        void dataChanged(const QModelIndex &, const QModelIndex &, const QVector<int> &);
};

#endif // PHOTOSWIDGET_HPP
