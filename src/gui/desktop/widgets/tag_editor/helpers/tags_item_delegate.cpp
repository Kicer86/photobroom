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
#include <QLineEdit>
#include <QCompleter>

#include "ui_utils/ieditor_factory.hpp"
#include "utils/variant_display.hpp"
#include "tags_model.hpp"


TagsItemDelegate::TagsItemDelegate():
    m_editorFactory(nullptr)
{

}


TagsItemDelegate::~TagsItemDelegate()
{

}


void TagsItemDelegate::setEditorFactory(IEditorFactory* editorFactory)
{
    m_editorFactory = editorFactory;
}


QWidget* TagsItemDelegate::createEditor(QWidget* parent_widget, const QStyleOptionViewItem &, const QModelIndex& index) const
{
    QWidget* const result = m_editorFactory->createEditor(index, parent_widget);

    return result;
}


QString TagsItemDelegate::displayText(const QVariant& value, const QLocale& locale) const
{
    return localize(value, locale);
}
