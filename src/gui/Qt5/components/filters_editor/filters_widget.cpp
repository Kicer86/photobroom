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

#include "filters_widget.hpp"

#include <QHBoxLayout>
#include <QComboBox>

FiltersWidget::FiltersWidget(QWidget* _parent): QWidget(_parent), m_sorting(nullptr)
{
    m_sorting = new QComboBox;
    connect(m_sorting, SIGNAL(currentIndexChanged(int)), this, SIGNAL(updateSorting(int)));

    QBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->addWidget(m_sorting);
    mainLayout->addStretch();
}


FiltersWidget::~FiltersWidget()
{

}

void FiltersWidget::setBasicFilters(const std::deque<QString>& items)
{
    for(const QString& item: items)
        m_sorting->addItem(item);
}


