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

#include "photos_widget.hpp"

#include <QPainter>

#include "info_widget.hpp"

#include "model_view/db_data_model.hpp"

PhotosWidget::PhotosWidget(QWidget* p): ImagesTreeView(p), m_dataModel(nullptr), m_infoWidget(nullptr)
{
    m_infoWidget = new InfoWidget( tr("foo"), this);
    m_infoWidget->hide();
}


PhotosWidget::~PhotosWidget()
{

}


void PhotosWidget::setModel(DBDataModel* dataModel)
{
    if (m_dataModel != nullptr)
        m_dataModel->disconnect(this);

    m_dataModel = dataModel;
    ImagesTreeView::setModel(dataModel);
}


QItemSelectionModel* PhotosWidget::getSelectionModel()
{
    return selectionModel();
}


DBDataModel* PhotosWidget::getModel()
{
    return m_dataModel;
}


QString PhotosWidget::getName()
{
    return ImagesTreeView::windowTitle();
}


void PhotosWidget::set(IConfiguration* configuration)
{
    ImagesTreeView::set(configuration);
}


void PhotosWidget::paintEvent(QPaintEvent* e)
{
    ImagesTreeView::paintEvent(e);

    const int rows = model()->rowCount();

    if (rows == 0)
    {
        const QRect w_r = rect();
        const QPoint w_c = w_r.center();

        QRect info = QRect(w_c, w_c);
        info = info.marginsAdded(QMargins(50, 30, 50, 30));

        m_infoWidget->resize(info.size());
        m_infoWidget->render(this->viewport(), info.topLeft());;
    }
}
