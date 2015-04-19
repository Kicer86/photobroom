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

#include <QTimeEdit>
#include <QDateEdit>
#include <QLineEdit>
#include <QTableWidget>
#include <QHeaderView>


struct TimeEditor: QTimeEdit
{
    explicit TimeEditor(QWidget* parent_widget = 0): QTimeEdit(parent_widget)
    {
        setDisplayFormat("hh:mm:ss");
    }
};


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


void ListEditor::addRow(int p)
{
    QLineEdit* e = new QLineEdit;

    insertRow(p);
    setCellWidget(p, 0, e);

    connect(e, SIGNAL(textEdited(QString)), this, SLOT(review()));
}


QString ListEditor::value(int r)
{
    QWidget* w = cellWidget(r, 0);
    assert(dynamic_cast<QLineEdit *>(w) != nullptr);
    QLineEdit* l = static_cast<QLineEdit *>(w);

    return l->text();
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


EditorFactory::EditorFactory(): QItemEditorFactory()
{
    QItemEditorCreatorBase *time_creator = new QStandardItemEditorCreator<TimeEditor>();
    registerEditor(QVariant::Time, time_creator);

    QItemEditorCreatorBase *list_creator = new QStandardItemEditorCreator<ListEditor>();
    registerEditor(QVariant::StringList, list_creator);
}


EditorFactory::~EditorFactory()
{

}
