/*
 * <one line to give the program's name and a brief idea of what it does.>
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

#include "level_editor.hpp"

#include <QHBoxLayout>
#include <QComboBox>

LevelEditor::LevelEditor(QWidget* _parent): QWidget(_parent), m_layout(nullptr)
{
    m_layout = new QHBoxLayout(this);

    updateGui();
}


LevelEditor::~LevelEditor()
{

}


void LevelEditor::updateGui()
{
    int size = m_layout->count();

    //make sure it's not empty
    if (size == 0)
    {
        m_layout->addWidget(new QComboBox);
        m_layout->addStretch();
    }

    //remove empty
    if (size > 2)
        for (int i = 0; i > size - 2;)    //do not check last combobox, it can be empty, and do not check stretch item (last layout item)
        {
            QWidget* rawWidget = m_layout->itemAt(i)->widget();
            QComboBox* comboBox = static_cast<QComboBox *>(rawWidget);

            const QString text = comboBox->currentText();
            if (text.isEmpty())
                m_layout->removeWidget(comboBox);
            else
                i++;
        }

    //add new if last is not empty
    size = m_layout->count();
    QWidget* rawWidget = m_layout->itemAt(size - 2)->widget();
    QLayoutItem* stretch = m_layout->itemAt(size - 1);
    QComboBox* comboBox = static_cast<QComboBox *>(rawWidget);

    const QString text = comboBox->currentText();
    if (text.isEmpty() == false)
    {
        m_layout->removeItem(stretch);
        m_layout->addWidget(new QComboBox);
        m_layout->addStretch();
    }
}
