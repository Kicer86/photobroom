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

#include <QComboBox>
#include <QHeaderView>
#include <QStringListModel>

#include <core/base_tags.hpp>

#include "tags_item_delegate.hpp"


TagsView::TagsView(IEditorFactory* editorFactory, QWidget* p):
    QTableView(p),
    m_editorFactory(),
    m_comboBox(nullptr)
{
    TagsItemDelegate* delegate = new TagsItemDelegate;
    delegate->setEditorFactory(editorFactory);

    verticalHeader()->hide();
    setItemDelegate(delegate);
    horizontalHeader()->setStretchLastSection(true);
}


TagsView::~TagsView()
{

}


void TagsView::setModel(QAbstractItemModel* model)
{
    connect(model, &QAbstractItemModel::columnsInserted,
            this, &TagsView::updateComboBox);

    connect(model, &QAbstractItemModel::columnsRemoved,
            this, &TagsView::updateComboBox);

    connect(model, &QAbstractItemModel::rowsInserted,
            this, &TagsView::updateComboBox);

    connect(model, &QAbstractItemModel::rowsRemoved,
            this, &TagsView::updateComboBox);

    connect(model, &QAbstractItemModel::modelReset,
            this, &TagsView::updateComboBox);

    QTableView::setModel(model);
}


bool TagsView::edit(const QModelIndex& index, QAbstractItemView::EditTrigger trigger, QEvent* e)
{
    const int rc = model()->rowCount();
    const int last_row = rc - 1;
    const bool is_last_item = index.row() == last_row;

    // allow edit in 2nd column and whole last row
    const bool status = index.column() == 1 || is_last_item?
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


void TagsView::updateRow(int row)
{
    QAbstractItemModel* m = QTableView::model();
    const QModelIndex item = m->index(row, 1);
    const QVariant d = item.data();
    const QVariant::Type t = d.type();

    if (t == QVariant::StringList)
        verticalHeader()->setSectionResizeMode(row, QHeaderView::ResizeToContents);
}


void TagsView::updateComboBox()
{
    QAbstractItemModel* m = model();

    if (m == nullptr)
        return;

    m_comboBox = new QComboBox(this);

    const auto tags = BaseTags::getAll();

    for(const auto& tag: tags)
    {
        const TagNameInfo info(tag);

        const QString text = BaseTags::getTr(tag);
        m_comboBox->addItem(text, QVariant::fromValue(info));
    }

    const int rc = m->rowCount();

    if (rc > 0)
    {
        const int last_row = rc - 1;
        const QModelIndex idx = model()->index(last_row, 0);

        setIndexWidget(idx, m_comboBox);
    }
}

