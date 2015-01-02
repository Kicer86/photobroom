/*
 * Generator of SELECT queries.
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

#ifndef SQLSELECTQUERYGENERATOR_H
#define SQLSELECTQUERYGENERATOR_H

#include <deque>

#include <QString>

#include <database/filter.hpp>

namespace Database
{

    class SqlSelectQueryGenerator
    {
        public:
            SqlSelectQueryGenerator();
            SqlSelectQueryGenerator(const SqlSelectQueryGenerator &) = delete;
            ~SqlSelectQueryGenerator();

            SqlSelectQueryGenerator& operator=(const SqlSelectQueryGenerator &) = delete;

            QString generate(const std::deque<IFilter::Ptr> &);
    };

}

#endif // SQLSELECTQUERYGENERATOR_H
