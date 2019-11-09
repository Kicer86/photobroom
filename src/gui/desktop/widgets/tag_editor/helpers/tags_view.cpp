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

#include <QKeyEvent>
#include <QHeaderView>
#include <QStringListModel>

#include <core/base_tags.hpp>

#include "tags_item_delegate.hpp"
#include "tags_model.hpp"              // TODO: TagsView and TagModel will be probably always used together,
                                       //       yet it would be nice to keep abstraction here

using namespace std::chrono;

TagsView::TagsView(IEditorFactory* editorFactory, QWidget* p):
    QTableView(p),
    m_editorFactory()
{
    TagsItemDelegate* delegate = new TagsItemDelegate(*editorFactory);

    verticalHeader()->hide();
    setItemDelegate(delegate);
    horizontalHeader()->setStretchLastSection(true);
}


bool TagsView::edit(const QModelIndex& index, QAbstractItemView::EditTrigger trigger, QEvent* e)
{
    // allow edit in 2nd column
    const bool status = index.column() == 1?
            QTableView::edit(index, trigger, e):
            false;

    return status;
}


void TagsView::rowsInserted(const QModelIndex& parent, int start, int end)
{
    QTableView::rowsInserted(parent, start, end);

    if (parent.isValid() == false)
        for(int i = start; i <= end; i++)
            updateRow(i);
}


int TagsView::sizeHintForRow(int row) const
{
    // TODO: remove .05 when https://bugreports.qt.io/browse/QTBUG-55514 is fixed
    const int default3 = verticalHeader()->defaultSectionSize() * 3.05;
    const int sizeHint = QTableView::sizeHintForRow(row);

    const int result = std::max(default3, sizeHint);

    return result;
}


void TagsView::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Delete)
    {
        const QModelIndexList indexes = selectedIndexes();

        QModelIndexList to_clean;

        for (const QModelIndex& index: indexes)
            if (index.column() == 1)
                to_clean.append(index);

        for(const QModelIndex& index: to_clean)
            model()->setData(index, QVariant());
    }
}


void TagsView::updateRow(int row)
{
    QAbstractItemModel* m = QTableView::model();
    const QModelIndex item = m->index(row, 1);
    const QVariant d = item.data();
    const QVariant::Type t = d.type();

    if (t == QVariant::StringList)
        verticalHeader()->setSectionResizeMode(row, QHeaderView::ResizeToContents);
}
