/*
 * Photo Broom - photos management tool.
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

#ifndef BASETAGS_HPP
#define BASETAGS_HPP

#include <map>
#include <vector>

#include <QCoreApplication>

#include "core_export.h"
#include "tag.hpp"


class CORE_EXPORT BaseTags
{
        Q_DECLARE_TR_FUNCTIONS(BaseTags)

    public:
        BaseTags() = delete;

        /// get list of all possible tag types
        static std::vector<TagTypes> getAll();

        /// Get translated text for tag
        static QString getTr(TagTypes);

        /// Get raw text for tag
        static QString getName(TagTypes);

        /// Get structure describing tag
        static Tag::ValueType getType(TagTypes);
};

#endif // BASETAGS_HPP
