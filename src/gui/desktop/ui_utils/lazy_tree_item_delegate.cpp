/*
 * LazyTreeItemDelegate - loads images from external source
 * Copyright (C) 2015  Michał Walenciak <MichalWalenciak@gmail.com>
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

#include "lazy_tree_item_delegate.hpp"

#include <QPainter>

#include <core/down_cast.hpp>

#include "models/aphoto_info_model.hpp"
#include "utils/ithumbnail_acquisitor.hpp"


LazyTreeItemDelegate::LazyTreeItemDelegate(ImagesTreeView* view):
    TreeItemDelegate(view),
    m_thumbnailAcquisitor()
{

}


LazyTreeItemDelegate::~LazyTreeItemDelegate()
{

}


void LazyTreeItemDelegate::set(IThumbnailAcquisitor* acquisitor)
{
    m_thumbnailAcquisitor = acquisitor;
}


QImage LazyTreeItemDelegate::getImage(const QModelIndex& idx, const QSize& size) const
{
    const QAbstractItemModel* model = idx.model();
    const APhotoInfoModel* photoInfoModel = down_cast<const APhotoInfoModel*>(model);      // TODO: not nice (see issue #177)
    const Photo::Data& details = photoInfoModel->getPhotoDetails(idx);

    const ThumbnailInfo info = { details.path, size.height() };
    QImage image = m_thumbnailAcquisitor->getThumbnail(info);

    if (details.groupInfo.role == GroupInfo::Representative)
    {
        const QSize canvasSize = image.size();
        QImage canvas(canvasSize, QImage::Format_ARGB32);
        canvas.fill(Qt::transparent);

        const QSize layerSize = canvasSize *0.9;
        const QImage layer_image = image.scaled(layerSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);

        const int layers = 5;
        const QSize size_diff = canvasSize - layerSize;
        const QPoint offset = QPoint(size_diff.width(), size_diff.height());
        const QPoint step = offset / (layers - 1);

        const float opacity_min = 0.3f;
        const float opacity_max = 1.0f;
        const float opacity_step = (opacity_max - opacity_min) / layers;

        float current_opacity = opacity_min;
        QPoint current_layer_pos;
        QPainter painter(&canvas);

        for(int i = 0; i < layers; i++)
        {
            QRect current_layer_rect(current_layer_pos, layerSize);
            current_layer_rect.moveTo(current_layer_pos);

            painter.setOpacity(current_opacity);
            painter.drawImage(current_layer_pos, layer_image);

            current_layer_pos += step;
            current_opacity += opacity_step;
        }

        image = canvas;
    }

    return image;
}
