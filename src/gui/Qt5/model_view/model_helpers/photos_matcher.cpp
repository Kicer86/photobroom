/*
 * class used for finding right place in tree for new photos
 * Copyright (C) 2014  Michał Walenciak <email>
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

#include "photos_matcher.hpp"

PhotosMatcher::PhotosMatcher(): m_idxDataManager(nullptr)
{

}


PhotosMatcher::~PhotosMatcher()
{

}


void PhotosMatcher::set(IdxDataManager* manager)
{
    m_idxDataManager = manager;
}


bool PhotosMatcher::doesMatchModelFilters(const IPhotoInfo::Ptr&) const
{

}


IdxData* PhotosMatcher::findParentFor(const IPhotoInfo::Ptr&) const
{

}
