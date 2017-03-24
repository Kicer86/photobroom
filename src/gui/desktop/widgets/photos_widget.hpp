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

#include "utils/thumbnail_acquisitor.hpp"

class QAbstractItemModel;
class QItemSelectionModel;
class QLineEdit;
class QVBoxLayout;

class DBDataModel;
class PhotosItemDelegate;
class MultiValueLineEdit;
class InfoBaloonWidget;
struct ImagesTreeView;

struct ICompleterFactory;
struct IConfiguration;
struct ILoggerFactory;
struct IPhotosManager;
struct ITaskExecutor;


class PhotosWidget: public QWidget
{
        Q_OBJECT

    public:
        PhotosWidget(QWidget * = nullptr);
        PhotosWidget(const PhotosWidget &) = delete;
        ~PhotosWidget();
        PhotosWidget& operator=(const PhotosWidget &) = delete;

        void set(ITaskExecutor *);
        void set(IPhotosManager *);
        void set(IConfiguration *);
        void set(ICompleterFactory *);
        void set(ILoggerFactory *);
        void setModel(DBDataModel *);

        QItemSelectionModel* viewSelectionModel() const;
        DBDataModel* getModel() const;

        void setBottomHintWidget(InfoBaloonWidget *);

    private:
        QTimer m_timer;
        ThumbnailAcquisitor m_thumbnailAcquisitor;
        std::unique_ptr<ILogger> m_thumbnailsLogger;
        DBDataModel* m_model;
        ImagesTreeView* m_view;
        PhotosItemDelegate* m_delegate;
        MultiValueLineEdit* m_searchExpression;
        QVBoxLayout* m_bottomHintLayout;
        ITaskExecutor* m_executor;

        void searchExpressionChanged(const QString &);
        void viewScrolled();
        void applySearchExpression();
        void thumbnailUpdated(const ThumbnailInfo &, const QImage &);

    signals:
        void performUpdate();
};

#endif // PHOTOSWIDGET_HPP
