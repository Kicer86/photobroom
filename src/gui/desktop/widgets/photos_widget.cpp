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

#include <configuration/iconfiguration.hpp>

#include "models/db_data_model.hpp"
#include "info_widget.hpp"
#include "config_keys.hpp"
#include <views/tree_item_delegate.hpp>


PhotosWidget::PhotosWidget(QWidget* p): ImagesTreeView(p), m_info(nullptr)
{
    m_info = new InfoBaloonWidget(this);
    m_info->hide();
    m_info->setText(tr("There are no photos in your collection.\n\nAdd some by choosing 'Add photos' action from 'Photos' menu."));
    m_info->adjustSize();

    setItemDelegate(new TreeItemDelegate(this));
}


PhotosWidget::~PhotosWidget()
{

}


void PhotosWidget::set(IConfiguration* configuration)
{
    const QVariant marginEntry = configuration->getEntry(ViewConfigKeys::itemsSpacing);
    assert(marginEntry.isValid());
    const int spacing = marginEntry.toInt();

    setSpacing(spacing);
}



void PhotosWidget::paintEvent(QPaintEvent* event)
{
    ImagesTreeView::paintEvent(event);

    // check if model is empty
    QAbstractItemModel* m = model();

    const bool empty = m->rowCount(QModelIndex()) == 0;

    if (empty && isEnabled())
    {
        // InfoWidget could be rendered directly on 'this', but it cannot due to bug:
        // https://bugreports.qt.io/browse/QTBUG-47302

        QPixmap infoPixMap(m_info->size());
        infoPixMap.fill(QColor(0, 0, 0, 0));
        m_info->render(&infoPixMap, QPoint(), QRegion(), 0);

        const QRect thisRect = rect();
        QRect infoRect = m_info->rect();
        infoRect.moveCenter(thisRect.center());

        QPainter painter(viewport());
        painter.drawPixmap(infoRect, infoPixMap);
    }
}
