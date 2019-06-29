/*
 * Extended version of QDateEdit
 * Copyright (C) 2019  Michał Walenciak <Kicer86@gmail.com>
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
 */

#include "qdate_edit_ex.hpp"

#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>


QDateEditEx::QDateEditEx(QWidget* p):
    QWidget(p),
    m_lineEdit(new QLineEdit(this))
{
    QHBoxLayout* l = new QHBoxLayout(this);
    QPushButton* cb = new QPushButton(this);

    l->addWidget(m_lineEdit);
    l->addWidget(cb);

    l->setMargin(0);
    l->setSpacing(0);

    m_lineEdit->setClearButtonEnabled(true);
}


