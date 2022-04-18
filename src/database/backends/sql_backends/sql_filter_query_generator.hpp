/*
 * Photo Broom - photos management tool.
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

#ifndef SQLFILTERQUERYGENERATOR_HPP
#define SQLFILTERQUERYGENERATOR_HPP

#include <vector>

#include <QString>

#include <database/filter.hpp>

namespace Database
{

    class SqlFilterQueryGenerator
    {
        public:
            SqlFilterQueryGenerator();
            SqlFilterQueryGenerator(const SqlFilterQueryGenerator &) = delete;
            ~SqlFilterQueryGenerator();

            SqlFilterQueryGenerator& operator=(const SqlFilterQueryGenerator &) = delete;

            QString generate(const Filter &) const;

        private:
            QString getFlagName(Photo::FlagsE flag) const;
            QString visit(const EmptyFilter &) const;
            QString visit(const GroupFilter& groupFilter) const;
            QString visit(const FilterPhotosWithTag& desciption) const;
            QString visit(const FilterPhotosWithFlags& flags) const;
            QString visit(const FilterNotMatchingFilter& filter) const;
            QString visit(const FilterPhotosWithId& filter) const;
            QString visit(const FilterPhotosMatchingExpression& filter) const;
            QString visit(const FilterPhotosWithPath& filter) const;
            QString visit(const FilterPhotosWithRole& filter) const;
            QString visit(const FilterPhotosWithPerson& personFilter) const;
            QString visit(const FilterPhotosWithGeneralFlag& genericFlagsFilter) const;
            QString visit(const FilterPhotosWithPHash& withPhashFilter) const;
            QString visit(const FilterSimilarPhotos& similarPhotosFilter) const;
    };

}

#endif // SQLFILTERQUERYGENERATOR_HPP
