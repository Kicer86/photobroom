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

#include <QItemEditorFactory>

#include <core/tag.hpp>

#include "utils/variant_display.hpp"


TagsItemDelegate::TagsItemDelegate()
{

}


TagsItemDelegate::~TagsItemDelegate()
{

}


QWidget* TagsItemDelegate::createEditor(QWidget* parent_widget, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QWidget* result = nullptr;
    QVariant data = index.data();
    const int multipleValuesType = qMetaTypeId<MultipleValues>();

    if (data.userType() == multipleValuesType)
    {
        MultipleValues multi = data.value<MultipleValues>();

        QItemEditorFactory* factory = itemEditorFactory();

        result = factory->createEditor(multi.userType(), parent_widget);
    }
    else
        result = QStyledItemDelegate::createEditor(parent_widget, option, index);

    return result;
}


QString TagsItemDelegate::displayText(const QVariant& value, const QLocale& locale) const
{
    return VariantDisplay()(value, locale);
}
