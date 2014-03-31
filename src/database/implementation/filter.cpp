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

#include "filter.hpp"

namespace Database
{

    Filter::Filter(): m_groups(), m_filters(), m_sortings()
    {

    }


    Filter::~Filter()
    {

    }


    void Filter::addGroup(const GroupDescription& group)
    {
        m_groups.push_back(group);
    }


    void Filter::addFilter(const FilterDescription& filter)
    {
        m_filters.push_back(filter);
    }


    void Filter::addSorting(const SortingDescription& sorting)
    {
        m_sortings.push_back(sorting);
    }

}
