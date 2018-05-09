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

#ifndef DRAGGABLETABLEWIDGET_HPP
#define DRAGGABLETABLEWIDGET_HPP

#include <QTableWidget>

class DraggableTableWidget: public QTableWidget
{
        Q_OBJECT

    public:
        DraggableTableWidget(QWidget *);
        ~DraggableTableWidget();

    private:
        QTableWidgetItem* m_current;

        void itemChosen(QTableWidgetItem *);
        void mouseReleaseEvent(QMouseEvent * event) override;
        void mouseMoveEvent(QMouseEvent * event) override;

    signals:
        void beginDrag(QTableWidgetItem *) const;
};

#endif // DRAGGABLETABLEWIDGET_HPP
