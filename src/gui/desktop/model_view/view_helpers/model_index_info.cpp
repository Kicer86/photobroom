/*
 * Model for view
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

#include "model_index_info.hpp"


void ModelIndexInfo::setRect(const QRect& r)
{
    rect = r;
    overallRect = QRect();          // not valid anymore
}


void ModelIndexInfo::setOverallRect(const QRect& r)
{
    overallRect = r;
}


const QRect& ModelIndexInfo::getRect() const
{
    return rect;
}


const QRect& ModelIndexInfo::getOverallRect() const
{
    return overallRect;
}


void ModelIndexInfo::cleanRects()
{
    rect = QRect();
    overallRect = QRect();
}


ModelIndexInfo::ModelIndexInfo(const QModelIndex& idx) : index(idx), expanded(false), rect(), overallRect()
{
    expanded = idx == QModelIndex();       //expand top root
}



///////////////////////////////////////////////////////////////////////////////


ModelIndexInfoSet::ModelIndexInfoSet(): m_model()
{

}


ModelIndexInfoSet::~ModelIndexInfoSet()
{

}

ModelIndexInfoSet::const_iterator ModelIndexInfoSet::find(const QModelIndex&) const
{
    return end();
}


ModelIndexInfoSet::const_iterator ModelIndexInfoSet::begin() const
{
    return tree_utils::make_recursive_iterator(m_model);
}


ModelIndexInfoSet::const_iterator ModelIndexInfoSet::end() const
{
    return tree_utils::make_recursive_iterator(m_model.cend(), m_model.cend());
}


ModelIndexInfoSet::iterator ModelIndexInfoSet::find(const QModelIndex&)
{
    return end();
}


ModelIndexInfoSet::iterator ModelIndexInfoSet::begin()
{
    return tree_utils::make_recursive_iterator(m_model);
}


ModelIndexInfoSet::iterator ModelIndexInfoSet::end()
{
    return tree_utils::make_recursive_iterator(m_model.end(), m_model.end());
}


void ModelIndexInfoSet::clear()
{
    m_model.clear();
}


void ModelIndexInfoSet::erase(const iterator&)
{

}


void ModelIndexInfoSet::insert(const ModelIndexInfo&)
{

}


void ModelIndexInfoSet::replace(const iterator&, const ModelIndexInfo&)
{

}


bool ModelIndexInfoSet::empty() const
{
    return true;
}


size_t ModelIndexInfoSet::size() const
{
    return 0;
}
