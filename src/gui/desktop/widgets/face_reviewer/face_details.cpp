/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2018  Michał Walenciak <Kicer86@gmail.com>
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

#include "face_details.hpp"

#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>


FaceDetails::FaceDetails(const QString& name, QWidget* p):
    QGroupBox(name, p),
    m_photo(nullptr),
    m_occurences(nullptr)
{
    QHBoxLayout* l = new QHBoxLayout(this);
    QVBoxLayout* dl = new QVBoxLayout;
    m_photo = new QLabel(this);
    m_occurences = new QLabel(this);

    dl->addWidget(m_occurences);

    l->addWidget(m_photo);
    l->addLayout(dl);
}


void FaceDetails::setOccurrences(int c)
{
    m_occurences->setText(tr("On %n photo(s)", "", c));
}


void FaceDetails::setModelPhoto(const QPixmap& p)
{
    m_photo->setPixmap(p);
}
