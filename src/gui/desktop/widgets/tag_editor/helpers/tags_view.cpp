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
#include "appendable_model_proxy.hpp"
#include "tags_model.hpp"              // TODO: TagsView and TagModel will be probably always used together,
                                       //       yet it would be nice to keep abstraction here


TagsView::TagsView(IEditorFactory* editorFactory, QWidget* p):
    QTableView(p),
    m_editorFactory(),
    m_comboBox(nullptr),
    m_proxy(new AppendableModelProxy(2, this))
{
    TagsItemDelegate* delegate = new TagsItemDelegate;
    delegate->setEditorFactory(editorFactory);

    verticalHeader()->hide();
    setItemDelegate(delegate);
    horizontalHeader()->setStretchLastSection(true);

    connect(m_proxy, &QAbstractItemModel::columnsInserted,
            this, &TagsView::setupComboBox);

    connect(m_proxy, &QAbstractItemModel::columnsRemoved,
            this, &TagsView::setupComboBox);

    connect(m_proxy, &QAbstractItemModel::rowsInserted,
            this, &TagsView::setupComboBox);

    connect(m_proxy, &QAbstractItemModel::rowsRemoved,
            this, &TagsView::setupComboBox);

    m_proxy->enableAppending(false);
    QTableView::setModel(m_proxy);
}


TagsView::~TagsView()
{
    if (m_comboBox)
        m_comboBox->disconnect(this);

    m_comboBox = nullptr;
}


void TagsView::setModel(QAbstractItemModel* model)
{
    m_proxy->setSourceModel(model);
    m_proxy->enableAppending(model != nullptr);

    setupComboBox();
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


std::set<BaseTagsList> TagsView::alreadyUsedTags() const
{
    std::set<BaseTagsList> result;

    const QAbstractItemModel* m = model();
    const int rc = m->rowCount();
    const int last_row = rc - (m_proxy->appendingEnabled()? 1: 0);  // do not collect data from row for appending

    for (int r = 0; r < last_row; r++)
    {
        const QModelIndex tagTypeIdx = m->index(r, 1);
        const QVariant tagInfoRoleRaw = tagTypeIdx.data(TagsModel::TagInfoRole);

        assert(tagInfoRoleRaw.isValid());
        const TagNameInfo tagInfoRole = tagInfoRoleRaw.value<TagNameInfo>();
        const BaseTagsList tag = tagInfoRole.getTag();

        result.insert(tag);
    }

    return result;
}


std::vector<BaseTagsList> TagsView::tagsNotUsed() const
{
    std::vector<BaseTagsList> notUsed;

    const std::set<BaseTagsList> used = alreadyUsedTags();
    const auto tags = BaseTags::getAll();

    for(const auto& tag: tags)
        if (used.find(tag) == used.end())
            notUsed.push_back(tag);

    return notUsed;
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


void TagsView::setupComboBox()
{
    QAbstractItemModel* m = model();

    if (m == nullptr)
        return;

    // fetch available tags
    const auto tags = tagsNotUsed();

    if (tags.empty())      // no tags - no combo
    {
        dropComboBox();
        m_proxy->enableAppending(false);
    }
    else
    {
        m_proxy->enableAppending(true);
        if (m_comboBox == nullptr)
        {
            m_comboBox = new QComboBox(this);

            connect(m_comboBox, &QComboBox::destroyed,
                    this, &TagsView::comboBoxDestroyed);

            connect(m_comboBox, qOverload<int>(&QComboBox::currentIndexChanged),
                    this, &TagsView::comboBoxChanged);

            placeWidget(m_comboBox);
        }
        else
            m_comboBox->clear();

        applyTags(tags);
    }
}


void TagsView::dropComboBox()
{
    if (m_comboBox)
        m_comboBox->disconnect(this);

    placeWidget(nullptr);
    m_comboBox = nullptr;
}


void TagsView::applyTags(const std::vector<BaseTagsList>& tags)
{
    for(const auto& tag: tags)
    {
        const TagNameInfo info(tag);

        const QString text = BaseTags::getTr(tag);
        m_comboBox->addItem(text, QVariant::fromValue(info));
    }
}


void TagsView::placeWidget(QWidget* w)
{
    QAbstractItemModel* m = model();
    const int rc = m->rowCount();

    if (rc > 0)
    {
        assert(w == nullptr || m_proxy->appendingEnabled());     // when we add widget, there must be extra row (added by proxy when appending is enabled)
        const int last_row = rc - (m_proxy->appendingEnabled()? 1: 0);
        const QModelIndex idx = m->index(last_row, 0);

        setIndexWidget(idx, w);
    }
    else if (w)
        w->deleteLater();
}


void TagsView::comboBoxChanged(int p)
{
    if (m_proxy->appendingEnabled() && m_comboBox)
    {
        const QVariant v = m_comboBox->itemData(p, TagsModel::TagInfoRole);
        const QVariant n = m_comboBox->itemText(p);
        const int rc = m_proxy->rowCount();
        const int last_row = rc - 1;
        const QModelIndex name_idx = m_proxy->index(last_row, 0);
        const QModelIndex value_idx = m_proxy->index(last_row, 1);

        m_proxy->setData(name_idx, n, Qt::DisplayRole);
        m_proxy->setData(value_idx, v, TagsModel::TagInfoRole);
    }
}


void TagsView::comboBoxDestroyed()
{
    m_comboBox = nullptr;

    // combo box was destroyed (model update usually)
    // recalculate where to place it now
    setupComboBox();
}
