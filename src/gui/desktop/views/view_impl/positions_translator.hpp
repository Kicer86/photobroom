/*
 * Utility for translating items relative positions to absolute ones.
 * Copyright (C) 2016  Michał Walenciak <MichalWalenciak@gmail.com>
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

#ifndef POSITIONSTRANSLATOR_HPP
#define POSITIONSTRANSLATOR_HPP

class PositionsTranslator
{
    public:
        PositionsTranslator();
        PositionsTranslator(const PositionsTranslator &) = delete;
        ~PositionsTranslator();

        PositionsTranslator& operator=(const PositionsTranslator &) = delete;
};

#endif // POSITIONSTRANSLATOR_HPP
