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


PositionsTranslator::PositionsTranslator(const Data* data): m_data(data)
{

}


PositionsTranslator::~PositionsTranslator()
{

}


QRect PositionsTranslator::getAbsoluteRect(const QModelIndex& idx) const
{
    auto mii = m_data->get(idx);

    QRect result = mii->second.getRect();
    QModelIndex parent = idx.parent();

    // top items have their posistions absolute by definition, no calculations needed
    if (parent.isValid())
    {
        const QRect parentRect = getAbsoluteRect(parent);
        result.translate(parentRect.bottomLeft());
    }

    return result;
}


QRect PositionsTranslator::getAbsoluteOverallRect(const QModelIndex& idx) const
{
    auto idxIt = m_data->get(idx);

    const QPoint position = getAbsolutePosition(idx);
    const ModelIndexInfo& info = idxIt->second;
    const QRect result(position, info.getOverallSize());

    return result;
}


QPoint PositionsTranslator::getAbsolutePosition(const QModelIndex& idx) const
{
    auto mii = m_data->get(idx);

    QPoint result = mii->second.getPosition();
    QModelIndex parent = idx.parent();

    // top items have their posistions absolute by definition, no calculations needed
    if (parent.isValid())
    {
        const QRect parentRect = getAbsoluteRect(parent);
        result += parentRect.bottomLeft();
    }

    return result;
}
