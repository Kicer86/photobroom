/*
 * Editor factory for views
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

#include "editor_factory.hpp"

#include <cassert>

#include <QCompleter>
#include <QDateEdit>
#include <QHeaderView>
#include <QLineEdit>
#include <QTableWidget>
#include <QTimeEdit>
#include <QTimer>
#include <QDoubleSpinBox>

#include <kratingwidget.h>
#include <kcolorcombo.h>

#include "icompleter_factory.hpp"
#include <core/base_tags.hpp>
#include <core/down_cast.hpp>
#include "widgets/tag_editor/helpers/tags_model.hpp"


namespace
{
    struct TimeEditor: QTimeEdit
    {
        explicit TimeEditor(QWidget* parent_widget = nullptr): QTimeEdit(parent_widget)
        {
            setDisplayFormat("hh:mm:ss");
        }
    };

    template<typename T>
    T* make_editor(ICompleterFactory* completerFactory, const TagTypeInfo& info, QWidget* parent)
    {
        T* editor = new T(parent);
        QCompleter* completer = completerFactory->createCompleter(info);
        completer->setParent(editor);
        editor->setCompleter(completer);

        return editor;
    }
}


EditorFactory::EditorFactory(): m_completerFactory(nullptr)
{

}


EditorFactory::~EditorFactory()
{

}


void EditorFactory::set(ICompleterFactory* completerFactory)
{
    m_completerFactory = completerFactory;
}


QWidget* EditorFactory::createEditor(const QModelIndex& index, QWidget* parent)
{
    const QVariant tagInfoRoleRaw = index.data(TagsModel::TagInfoRole);
    const TagTypeInfo tagInfoRole = tagInfoRoleRaw.value<TagTypeInfo>();

    return createEditor(tagInfoRole, parent);
}


QWidget* EditorFactory::createEditor(const TagTypeInfo& info, QWidget* parent)
{
    QWidget* result = nullptr;

    const auto tagType = info.getTag();

    switch(tagType)
    {
        case TagTypes::Event:
        case TagTypes::Place:
            result = make_editor<QLineEdit>(m_completerFactory, info, parent);
            break;

        case TagTypes::Date:
            result = new QDateEdit(parent);
            break;

        case TagTypes::Time:
            result = new TimeEditor(parent);
            break;

        case TagTypes::Rating:
            result = new KRatingWidget(parent);
            break;

        case TagTypes::Category:
            result = new KColorCombo(parent);
            break;

        case TagTypes::Invalid:
        case TagTypes::_People:
            assert(!"Unexpected call");
            break;
    }

    return result;
}


QByteArray EditorFactory::valuePropertyName(const TagTypeInfo& info) const
{
    QByteArray result;

    const auto tagType = info.getTag();

    switch(tagType)
    {
        case TagTypes::Event:
        case TagTypes::Place:
            result = "text";
            break;

        case TagTypes::Date:
            result = "date";
            break;

        case TagTypes::Time:
            result = "time";
            break;

        case TagTypes::Rating:
            result = "rating";
            break;

        case TagTypes::Category:
            result = "color";
            break;

        case TagTypes::Invalid:
        case TagTypes::_People:
            assert(!"Unexpected call");
            break;
    }

    return result;
}
