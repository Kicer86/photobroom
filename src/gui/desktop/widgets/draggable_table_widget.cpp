/*
 * QTableWidget with dragagable items
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

#include "draggable_table_widget.hpp"


DraggableTableWidget::DraggableTableWidget(QWidget* p):
    QTableWidget(p),
    m_current(nullptr)
{
    connect(this, &QTableWidget::itemPressed,
            this, &DraggableTableWidget::itemChosen);
}


DraggableTableWidget::~DraggableTableWidget()
{

}


void DraggableTableWidget::itemChosen(QTableWidgetItem* item)
{
    m_current = item;
}


void DraggableTableWidget::mouseReleaseEvent(QMouseEvent* event)
{
    QTableWidget::mouseReleaseEvent(event);
    m_current = nullptr;
}


void DraggableTableWidget::mouseMoveEvent(QMouseEvent* event)
{
    if (m_current == nullptr)
        QTableWidget::mouseMoveEvent(event);
    else
        emit beginDrag(m_current);
}
