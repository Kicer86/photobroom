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

#include "positions_translator.hpp"

#include "model_index_info.hpp"
#include "data.hpp"


PositionsTranslator::PositionsTranslator(Data* data): m_data(data)
{

}


PositionsTranslator::~PositionsTranslator()
{

}


QRect PositionsTranslator::getAbsoluteRect(const Data::ModelIndexInfoSet::const_level_iterator& mii) const
{
    QRect result = mii->getRect();
    auto parent = mii.parent();

    // top items have their posistions absolute by definition, no calculations needed
    if (parent != m_data->getModel().cend())
    {
        QRect parentRect = getAbsoluteRect(parent);
        result.translate(parentRect.topLeft());
    }

    return result;
}


QPoint PositionsTranslator::getAbsolutePosition(const Data::ModelIndexInfoSet::const_level_iterator& mii) const
{
    QPoint result = mii->getPosition();
    auto parent = mii.parent();

    // top items have their posistions absolute by definition, no calculations needed
    if (parent != m_data->getModel().cend())
    {
        QPoint parentPosition = getAbsolutePosition(parent);
        result += parentPosition;
    }

    return result;
}
