/*
 * View for tags
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

#include "tags_view.hpp"

#include <QStringListModel>
#include <QHeaderView>

TagsView::TagsView(QWidget* p): QTableView(p)
{
    verticalHeader()->hide();
}


TagsView::~TagsView()
{

}


bool TagsView::edit(const QModelIndex& index, QAbstractItemView::EditTrigger trigger, QEvent* e)
{
    const bool status = index.column() == 0?
            false:
            QAbstractItemView::edit(index, trigger, e);

    return status;
}

