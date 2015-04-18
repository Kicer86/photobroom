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


struct ListEditor: QTableWidget
{
    explicit ListEditor(QWidget* parent_widget = 0): QTableWidget(parent_widget)
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

    private:
        void addRow(int p)
        {
            insertRow(p);
            setCellWidget(p, 0, new QLineEdit);
        }

        // QWidget overrides
        virtual QSize minimumSizeHint() const override
        {
            const QSize result = sizeHint();

            return result;
        }

    private slots:
        void review()
        {
            if(rowCount() == 0)
                addRow(0);
        }
};


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
