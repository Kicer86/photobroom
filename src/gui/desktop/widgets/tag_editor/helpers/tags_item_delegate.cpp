/*
 * Item delagate for tags view
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

#include "tags_item_delegate.hpp"

#include <QCompleter>
#include <QItemEditorFactory>
#include <QLineEdit>
#include <QPainter>

#include <kratingpainter.h>

#include "ui_utils/ieditor_factory.hpp"
#include "utils/variant_display.hpp"
#include "tags_model.hpp"


TagsItemDelegate::TagsItemDelegate(IEditorFactory& editorFactory):
    m_editorFactory(editorFactory)
{

}


TagsItemDelegate::~TagsItemDelegate()
{

}


QWidget* TagsItemDelegate::createEditor(QWidget* parent_widget, const QStyleOptionViewItem &, const QModelIndex& index) const
{
    QWidget* const result = m_editorFactory.createEditor(index, parent_widget);

    return result;
}


QString TagsItemDelegate::displayText(const QVariant& value, const QLocale& locale) const
{
    return localize(value, locale);
}


void TagsItemDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
    const QVariant tagInfoRoleRaw = index.data(TagsModel::TagInfoRole);
    const TagTypeInfo tagInfoRole = tagInfoRoleRaw.value<TagTypeInfo>();
    const QByteArray property = m_editorFactory.valuePropertyName(tagInfoRole);

    model->setData(index, editor->property(property), Qt::EditRole);
}


void TagsItemDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    const QVariant tagInfoRoleRaw = index.data(TagsModel::TagInfoRole);
    const TagTypeInfo tagInfoRole = tagInfoRoleRaw.value<TagTypeInfo>();
    const QByteArray property = m_editorFactory.valuePropertyName(tagInfoRole);
    const QVariant value = index.data(Qt::EditRole);

    editor->setProperty(property, value);
}


void TagsItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    if ( (option.state & QStyle::State_Editing) == 0)
    {
        const QVariant tagInfoRoleRaw = index.data(TagsModel::TagInfoRole);
        const TagTypeInfo tagInfoRole = tagInfoRoleRaw.value<TagTypeInfo>();
        const TagTypes tagType = tagInfoRole.getTag();

        if (tagType == TagTypes::Rating)
        {
            const QVariant value = index.data(Qt::EditRole);
            KRatingPainter().paint(painter, option.rect, value.toInt());
        }
        else if (tagType == TagTypes::Category)
        {
            const QVariant value = index.data(Qt::EditRole);
            const QColor color = value.value<QColor>();
            painter->setBrush(color);
            painter->drawRect(option.rect);
        }
        else
            QStyledItemDelegate::paint(painter, option, index);
    }
}
