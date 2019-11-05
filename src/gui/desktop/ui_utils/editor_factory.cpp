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

#include <core/down_cast.hpp>
#include "widgets/tag_editor/helpers/tags_model.hpp"
#include "icompleter_factory.hpp"
#include <QDoubleSpinBox>


namespace
{
    struct TimeEditor: QTimeEdit
    {
        explicit TimeEditor(QWidget* parent_widget = 0): QTimeEdit(parent_widget)
        {
            setDisplayFormat("hh:mm:ss");
        }
    };

    template<typename T>
    T* make_editor(ICompleterFactory* completerFactory, const TagNameInfo& info, QWidget* parent)
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
    const TagNameInfo tagInfoRole = tagInfoRoleRaw.value<TagNameInfo>();

    return createEditor(tagInfoRole, parent);
}


QWidget* EditorFactory::createEditor(const TagNameInfo& info, QWidget* parent)
{
    QWidget* result = nullptr;

    switch(info.getType())
    {
        case Tag::Type::String:
            result = make_editor<QLineEdit>(m_completerFactory, info, parent);
            break;

        case Tag::Type::Date:
            result = new QDateEdit(parent);
            break;

        case Tag::Type::Time:
            result = new TimeEditor(parent);
            break;

        case Tag::Type::Float:
            result = new QDoubleSpinBox(parent);
            break;

        case Tag::Type::Uint64:
            result = new QLineEdit(parent);
            break;

        case Tag::Type::Empty:
            assert(!"Unexpected call");
            break;
    }

    return result;
}


QByteArray EditorFactory::valuePropertyName(const TagNameInfo& info) const
{
    QByteArray result;

    switch(info.getType())
    {
        case Tag::Type::String:
            result = "text";
            break;

        case Tag::Type::Date:
            result = "date";
            break;

        case Tag::Type::Time:
            result = "time";
            break;

        case Tag::Type::Float:
            result = "value";
            break;

        case Tag::Type::Uint64:
            result = "text";
            break;

        case Tag::Type::Empty:
            assert(!"Unexpected call");
            break;
    }

    return result;
}
