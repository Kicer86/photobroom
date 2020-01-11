/*
 * Photo Broom - photos management tool.
 * Copyright (C) 2018  Michał Walenciak <Kicer86@gmail.com>
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
 */

#include "table_widget_with_limited_drop.hpp"

#include <QDragMoveEvent>

TableWidgetWithLimitedDrop::TableWidgetWithLimitedDrop(QWidget* p):
    QTableWidget(p)
{

}


void TableWidgetWithLimitedDrop::dragMoveEvent(QDragMoveEvent * event)
{
    // check if allowed by base
    QTableWidget::dragMoveEvent(event);

    if (event->isAccepted())
    {
        const QPoint pos = event->pos();
        QTableWidgetItem* i = itemAt(pos);

        if (i == nullptr)
            event->ignore();
        else
        {
            const auto keys = event->keyboardModifiers();
            if (keys == Qt::NoModifier)
                event->accept();
            else
                event->ignore();
        }
    }
}
