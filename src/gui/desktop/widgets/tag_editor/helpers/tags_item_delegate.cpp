/*
 * Photo Broom - photos management tool.
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

#include "ui_utils/ieditor_factory.hpp"
#include "utils/svg_utils.hpp"
#include "utils/variant_display.hpp"
#include "tags_model.hpp"


TagsItemDelegate::TagsItemDelegate(IEditorFactory& editorFactory):
    m_editorFactory(editorFactory)
{
    QImage star = SVGUtils::load(":/gui/star.svg", {32, 32}, QColor(0, 0, 0, 0));
    m_ratingPainter.setCustomPixmap(QPixmap::fromImage(star));
}


TagsItemDelegate::~TagsItemDelegate()
{

}


QWidget* TagsItemDelegate::createEditor(QWidget* parent_widget, const QStyleOptionViewItem &, const QModelIndex& index) const
{
    QWidget* const result = m_editorFactory.createEditor(index, parent_widget);
    result->setAutoFillBackground(true);

    return result;
}


QString TagsItemDelegate::displayText(const QVariant& value, const QLocale& locale) const
{
    return Variant::localize(value, locale);
}


void TagsItemDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
    const QVariant tagTypeRoleRaw = index.data(TagsModel::TagTypeRole);
    const Tag::Types tagTypeRole = tagTypeRoleRaw.value<Tag::Types>();
    const QByteArray property = m_editorFactory.valuePropertyName(tagTypeRole);

    model->setData(index, editor->property(property), Qt::EditRole);
}


void TagsItemDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    const QVariant tagTypeRoleRaw = index.data(TagsModel::TagTypeRole);
    const Tag::Types tagTypeRole = tagTypeRoleRaw.value<Tag::Types>();
    const QByteArray property = m_editorFactory.valuePropertyName(tagTypeRole);
    const QVariant value = index.data(Qt::EditRole);

    editor->setProperty(property, value);
}


void TagsItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    if ( (option.state & QStyle::State_Editing) == 0)
    {
        const QVariant tagTypeRoleRaw = index.data(TagsModel::TagTypeRole);
        const Tag::Types tagType = tagTypeRoleRaw.value<Tag::Types>();

        if (tagType == Tag::Types::Rating)
        {
            const QVariant value = index.data(Qt::EditRole);

            if (value.isNull() == false)
                m_ratingPainter.paint(painter, option.rect, value.toInt());
        }
        else if (tagType == Tag::Types::Category)
        {
            const QVariant value = index.data(Qt::EditRole);

            if (value.isNull() == false)
            {
                const QColor color = value.value<QColor>();
                painter->setBrush(color);
                painter->drawRect(option.rect);
            }
        }
        else
            QStyledItemDelegate::paint(painter, option, index);
    }
}
