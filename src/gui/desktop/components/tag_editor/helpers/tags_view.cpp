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

#include <cassert>

#include <QStringListModel>
#include <QHeaderView>

#include "tags_item_delegate.hpp"


TagsView::TagsView(QWidget* p): QTableView(p), m_editorFactory()
{
    TagsItemDelegate* delegate = new TagsItemDelegate;
    delegate->setItemEditorFactory(&m_editorFactory);

    verticalHeader()->hide();
    setItemDelegate(delegate);
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


void TagsView::rowsInserted(const QModelIndex& parent, int start, int end)
{
    QTableView::rowsInserted(parent, start, end);

    if (parent.isValid() == false)
        for(int i = start; i <= end; i++)
            updateRow(i);
}


void TagsView::dataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles)
{
    QTableView::dataChanged(topLeft, bottomRight, roles);

    for(int i = topLeft.row(); i <= bottomRight.row(); i++)
        updateRow(i);
}


void TagsView::updateRow(int row)
{
    QAbstractItemModel* m = QTableView::model();
    const QModelIndex item = m->index(row, 1);
    const QVariant d = item.data();
    const QVariant::Type t = d.type();

    if (t == QVariant::StringList)
    {
        QHeaderView* hv = verticalHeader();

        const int s_default3 = hv->defaultSectionSize() * 3;
        const int s_adjusted = sizeHintForRow(row);
        const int n_size = std::max(s_default3, s_adjusted);

        hv->resizeSection(row, n_size);
    }
}
