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

#ifndef PHOTOSWIDGET_HPP
#define PHOTOSWIDGET_HPP

#include <QWidget>
#include <QSet>
#include <QTimer>

#include <core/thumbnail_manager.hpp>
#include <database/idatabase.hpp>

#include "ui_utils/lazy_tree_item_delegate.hpp"


class QAbstractItemModel;
class QItemSelectionModel;
class QLineEdit;
class QVBoxLayout;

class DBDataModel;
class PhotosItemDelegate;
class MultiValueLineEdit;
class InfoBalloonWidget;
class ImagesTreeView;

struct ICompleterFactory;
struct IConfiguration;
struct ITaskExecutor;
struct IThumbnailsManager;


class PhotosWidget: public QWidget, IImagesSource
{
        Q_OBJECT

    public:
        PhotosWidget(QWidget * = nullptr);
        PhotosWidget(const PhotosWidget &) = delete;
        ~PhotosWidget();
        PhotosWidget& operator=(const PhotosWidget &) = delete;

        void set(IThumbnailsManager *);
        void set(ITaskExecutor *);
        void set(IConfiguration *);
        void set(ICompleterFactory *);
        void setDB(Database::IDatabase *);
        void setModel(DBDataModel *);

        QItemSelectionModel* viewSelectionModel() const;
        DBDataModel* getModel() const;

        void setBottomHintWidget(InfoBalloonWidget *);

    private:
        QTimer m_timer;
        QSet<QModelIndex> m_waitingForThumbnails;
        DBDataModel* m_model;
        ImagesTreeView* m_view;
        PhotosItemDelegate* m_delegate;
        MultiValueLineEdit* m_searchExpression;
        QVBoxLayout* m_bottomHintLayout;
        ITaskExecutor* m_executor;
        IThumbnailsManager* m_thumbnailsManager;

        void searchExpressionChanged(const QString &);
        void viewScrolled();
        void applySearchExpression();
        void thumbnailUpdated(const QModelIndex &);

        // IImagesSource:
        QImage image(const QModelIndex &, const QSize &) override;

    signals:
        void performUpdate();
};

#endif // PHOTOSWIDGET_HPP
