/*
 * Stuff for defining filters and sorting order for database queries.
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

#ifndef FILTER_H
#define FILTER_H

#include <memory>
#include <vector>

namespace Database
{

    struct GroupDescription
    {

    };

    struct FilterDescription
    {

    };

    struct SortingDescription
    {

    };


    class Filter
    {
        public:
            Filter();
            Filter(const Filter &) = delete;
            ~Filter();

            Filter& operator=(const Filter &) = delete;
            bool operator==(const Filter &) = delete;

            void addGroup(const GroupDescription &);
            void addFilter(const FilterDescription &);
            void addSorting(const SortingDescription &);

            template<typename T>
            void addGroups(const T& container)
            {
                for(const auto& item: container)
                    addGroup(item);
            }

            template<typename T>
            void addFilters(const T& container)
            {
                for(const auto& item: container)
                    addFilter(item);
            }

            template<typename T>
            void addSortings(const T& container)
            {
                for(const auto& item: container)
                    addSorting(item);
            }

        private:
            std::vector<GroupDescription> m_groups;
            std::vector<FilterDescription> m_filters;
            std::vector<SortingDescription> m_sortings;
    };
}
#endif // FILTER_H
