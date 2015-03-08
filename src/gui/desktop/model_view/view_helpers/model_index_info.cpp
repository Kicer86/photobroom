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

namespace
{
    template<typename T, typename M>
    T _find(M& model, const std::vector<size_t>& hierarchy)
    {
        assert(hierarchy.empty() == false);

        //setup first item
        T item_it = model.end();

        if (model.empty() == false)
            for(size_t i = 0; i < hierarchy.size(); i++)
            {
                const size_t pos = hierarchy[i];

                if (i == 0)
                {
                    T b(model.begin());
                    T e(model.end());

                    const size_t c = e - b;               //how many items?
                    if (pos < c)
                        item_it = T(model.begin()) + pos;
                    else
                    {
                        item_it = model.end();
                        break;                            //out of scope
                    }
                }
                else
                {
                    const size_t c = item_it->children_count();

                    if (pos < c)
                        item_it = item_it->begin() + pos;
                    else
                    {
                        item_it = model.end();
                        break;                            //out of scope
                    }
                }
            }

        return item_it;
    }
}


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


ModelIndexInfo::ModelIndexInfo(): expanded(false), rect(), overallRect()
{
}



///////////////////////////////////////////////////////////////////////////////


ModelIndexInfoSet::ModelIndexInfoSet(): m_model()
{

}


ModelIndexInfoSet::~ModelIndexInfoSet()
{

}

ModelIndexInfoSet::const_iterator ModelIndexInfoSet::find(const QModelIndex& index) const
{
    std::vector<size_t> hierarchy = generateHierarchy(index);

    return _find<const_flat_iterator>(m_model, hierarchy);
}


ModelIndexInfoSet::const_iterator ModelIndexInfoSet::begin() const
{
    return m_model.begin();
}


ModelIndexInfoSet::const_iterator ModelIndexInfoSet::end() const
{
    return m_model.end();
}


ModelIndexInfoSet::const_iterator ModelIndexInfoSet::cbegin() const
{
    return m_model.cbegin();
}


ModelIndexInfoSet::const_iterator ModelIndexInfoSet::cend() const
{
    return m_model.cend();
}


ModelIndexInfoSet::iterator ModelIndexInfoSet::find(const QModelIndex& index)
{
    std::vector<size_t> hierarchy = generateHierarchy(index);

    return _find<flat_iterator>(m_model, hierarchy);
}


ModelIndexInfoSet::iterator ModelIndexInfoSet::begin()
{
    return m_model.begin();
}


ModelIndexInfoSet::iterator ModelIndexInfoSet::end()
{
    return m_model.end();
}


void ModelIndexInfoSet::clear()
{
    m_model.clear();
}


void ModelIndexInfoSet::erase(const iterator& it)
{
    m_model.erase(it);
}


//TODO: simplify and mix with find()
ModelIndexInfoSet::iterator ModelIndexInfoSet::insert(const QModelIndex& index, const ModelIndexInfo& info)
{
    auto it = find(index);
    
    if (it == end())
    {
        std::vector<size_t> hierarchy = generateHierarchy(index);
        const size_t hierarchy_size = hierarchy.size();
        assert(hierarchy_size > 0);

        //setup first item
        flat_iterator item_it = m_model.end();

        for(size_t i = 0; i < hierarchy.size(); i++)
        {
            const size_t pos = hierarchy[i];
            const bool last = i + 1 == hierarchy_size;

            if (i == 0)
            {
                flat_iterator b(m_model.begin());
                flat_iterator e(m_model.end());

                const size_t c = e - b;               //how many items?
                if (pos < c)
                    item_it = flat_iterator(m_model.begin()) + hierarchy[i];
                else if (pos == c)                    //just append after last item?
                {
                    if (last == false)                //for last level do nothing - we will inster this item later below
                    {
                        flat_iterator ins = b + pos;
                        item_it = m_model.insert(ins, ModelIndexInfo());
                    }
                }
                else
                    assert(!"missing siblings");
            }
            else
            {
                const size_t c = item_it->children_count();
                if (pos < c)
                    item_it = item_it->begin() + pos;
                else if (pos == c)                    //just append after last item?
                {
                    flat_iterator ins = item_it->begin() + pos;

                    if (last)
                        item_it = ins;                // for last level of hierarchy set item_it to desired position
                    else
                        item_it = m_model.insert(ins, ModelIndexInfo());
                }
                else
                    assert(!"missing siblings");
            }
        }

        it = m_model.insert(item_it, info);
    }
    else
        *it = info;
    
    return it;
}


bool ModelIndexInfoSet::empty() const
{
    return m_model.empty();
}


size_t ModelIndexInfoSet::size() const
{
    return m_model.cend() - m_model.cbegin();
}


std::vector<size_t> ModelIndexInfoSet::generateHierarchy(const QModelIndex& index) const
{
    std::vector<size_t> result;
    
    if (index.isValid())
    {
        std::vector<size_t> parents = generateHierarchy(index.parent());
        result.insert(result.begin(), parents.cbegin(), parents.cend());
        result.push_back(index.row());
    }
    else
        result.push_back(0);             //top item
    
    return result;
}
