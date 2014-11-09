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

#include "model_view/db_data_model.hpp"

PhotosWidget::PhotosWidget(QWidget* p): ImagesTreeView(p)
{

}


PhotosWidget::~PhotosWidget()
{

}


QString PhotosWidget::getName()
{
    return ImagesTreeView::windowTitle();
}


QAbstractItemView* PhotosWidget::getView()
{
    return this;
}


void PhotosWidget::set(IConfiguration* configuration)
{
    ImagesTreeView::set(configuration);
}

