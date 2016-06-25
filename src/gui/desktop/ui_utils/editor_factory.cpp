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

#include <core/down_cast.hpp>
#include "widgets/tag_editor/helpers/tags_model.hpp"
#include "icompleter_factory.hpp"

namespace
{
    struct TimeEditor: QTimeEdit
    {
        explicit TimeEditor(QWidget* parent_widget = 0): QTimeEdit(parent_widget)
        {
            setDisplayFormat("hh:mm:ss");
        }
    };
}


///////////////////////////////////////////////////////////////////////////////


ListEditor::ListEditor(QWidget* parent_widget): QTableWidget(parent_widget)
{
    setColumnCount(1);
    horizontalHeader()->hide();
    horizontalHeader()->setStretchLastSection(true);
    verticalHeader()->hide();
    setGridStyle(Qt::NoPen);
    setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    setFrameShape(QFrame::NoFrame);
    setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum));

    review();
}


QStringList ListEditor::getValues() const
{
    QStringList result;
    const int rows = rowCount();

    for(int r = 0; r < rows; r++)
    {
        const QString v = value(r);

        if (v.isEmpty() == false)
            result.push_back(v);
    }

    return result;
}


void ListEditor::setValues(const QStringList& values)
{
    for(int i = 0; i < values.size(); i++)
        setValue(i, values[i]);
}


void ListEditor::addRow(int p)
{
    QLineEdit* e = new QLineEdit;

    insertRow(p);
    setCellWidget(p, 0, e);

    connect(e, SIGNAL(textChanged(QString)), this, SLOT(review()));
}


QString ListEditor::value(int r) const
{
    QWidget* w = cellWidget(r, 0);
    QLineEdit* l = down_cast<QLineEdit *>(w);

    return l->text();
}


void ListEditor::setValue(int r, const QString& v)
{
    QWidget* w = cellWidget(r, 0);
    QLineEdit* l = down_cast<QLineEdit *>(w);

    l->setText(v);
}


QSize ListEditor::minimumSizeHint() const
{
    const QSize result = sizeHint();

    return result;
}


void ListEditor::review()
{
    int r = rowCount();
    if (r == 0)
        addRow(0);

    if (r > 0)
    {
        const QString v1 = value(r - 1);
        if (v1.isEmpty() == false)                   // last row not empty? add new one
            addRow(r);
    }

    for(; r > 1; r--)
    {
        const QString v1 = value(r - 1);
        const QString v2 = value(r - 2);

        if (v1.isEmpty() && v2.isEmpty())            // two last rows empty? remove last one and continue looping with r-1 elements
            removeRow(r - 1);
        else
            break;                                   // otherwise - stop

    }
}

///////////////////////////////////////////////////////////////////////////////


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
        case TagNameInfo::Text:
        {
            QLineEdit* le = new QLineEdit(parent);
            QCompleter* completer = m_completerFactory->createCompleter(info);
            completer->setParent(le);
            le->setCompleter(completer);

            result = le;
            break;
        }

        case TagNameInfo::Date:
            result = new QDateEdit(parent);
            break;

        case TagNameInfo::Time:
            result = new TimeEditor(parent);
            break;

        case TagNameInfo::List:
            result = new ListEditor(parent);
            break;

        case TagNameInfo::Invalid:
            assert(!"Unknown type");
            break;
    }

    return result;
}


QByteArray EditorFactory::valuePropertyName(const TagNameInfo::Type& type) const
{
    QByteArray result;

    switch(type)
    {
        case TagNameInfo::Text:
            result = "text";
            break;

        case TagNameInfo::Date:
            result = "date";
            break;

        case TagNameInfo::Time:
            result = "time";
            break;

        case TagNameInfo::List:
            result = "value";
            break;

        case TagNameInfo::Invalid:
            assert(!"Unknown type");
            break;
    }

    return result;
}
