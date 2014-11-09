/*
 * Widget for Staging area.
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

#ifndef STAGEDPHOTOSWIDGET_HPP
#define STAGEDPHOTOSWIDGET_HPP

#include <memory>

#include <QWidget>

#include "iview.hpp"

struct IConfiguration;
struct ITaskExecutor;
class StagedPhotosDataModel;
class ImagesTreeView;

class StagedPhotosWidget: public QWidget, public IView
{
        Q_OBJECT

    public:
        StagedPhotosWidget(QWidget * = nullptr);
        StagedPhotosWidget(const StagedPhotosWidget &) = delete;
        ~StagedPhotosWidget();
        StagedPhotosWidget& operator=(const StagedPhotosWidget &) = delete;

        void setModel(StagedPhotosDataModel *);
        void set(IConfiguration *);

        // IView:
        QString getName() override;
        QAbstractItemView* getView() override;

    private:
        ImagesTreeView* m_view;
        StagedPhotosDataModel* m_dataModel;


    private slots:
        void applyChanges();
};

#endif // STAGEDPHOTOSWIDGET_HPP
