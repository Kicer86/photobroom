/*
 * Copyright (C) 2020  Michał Walenciak <Kicer86@gmail.com>
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

#include "photos_model_component.hpp"

#include "models/flat_model.hpp"


PhotosModelComponent::PhotosModelComponent(QObject* p)
    : QObject(p)
    , m_model(nullptr)
{
}


FlatModel* PhotosModelComponent::model() const
{
    return m_model;
}


void PhotosModelComponent::setModel(FlatModel* m)
{
    m_model = m;

    emit modelChanged();
}
