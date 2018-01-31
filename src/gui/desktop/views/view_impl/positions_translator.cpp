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


QRect PositionsTranslator::getAbsoluteRect(const Data::ModelIndexInfoSet::Model::const_level_iterator& mii) const
{
    QRect result = mii->getRect();
    auto parent = mii.parent();

    // top items have their posistions absolute by definition, no calculations needed
    if (parent != m_data->getModel().cend())
    {
        const QRect parentRect = getAbsoluteRect(parent);
        result.translate(parentRect.bottomLeft());
    }

    return result;
}


QRect PositionsTranslator::getAbsoluteRect(const QModelIndex& idx) const
{
    const ModelIndexInfo idxInfo = m_data->get(idx);
    const QModelIndex parent = idx.parent();

    QRect result = idxInfo.getRect();

    // top items have their posistions absolute by definition, no calculations needed
    if (parent.isValid())
    {
        const QRect parentRect = getAbsoluteRect(parent);
        result.translate(parentRect.bottomLeft());
    }

    return result;
}


QRect PositionsTranslator::getAbsoluteOverallRect(const Data::ModelIndexInfoSet::Model::const_iterator& mii) const
{
    const QPoint position = getAbsolutePosition(mii);
    const ModelIndexInfo& info = *mii;
    const QRect result(position, info.getOverallSize());

    return result;
}


QRect PositionsTranslator::getAbsoluteOverallRect(const QModelIndex& idx) const
{
    const QPoint position = getAbsolutePosition(idx);
    const ModelIndexInfo& info = m_data->get(idx);
    const QRect result(position, info.getOverallSize());

    return result;
}


QPoint PositionsTranslator::getAbsolutePosition(const Data::ModelIndexInfoSet::Model::const_level_iterator& mii) const
{
    QPoint result = mii->getPosition();
    auto parent = mii.parent();

    // top items have their posistions absolute by definition, no calculations needed
    if (parent != m_data->getModel().cend())
    {
        const QRect parentRect = getAbsoluteRect(parent);
        result += parentRect.bottomLeft();
    }

    return result;
}


QPoint PositionsTranslator::getAbsolutePosition(const QModelIndex& idx) const
{
    const ModelIndexInfo& info = m_data->get(idx);

    QPoint result = info.getPosition();
    const QModelIndex& parentIdx = idx.parent();

    // top items have their posistions absolute by definition, no calculations needed
    if (parentIdx.isValid())
    {
        const QRect parentRect = getAbsoluteRect(parentIdx);
        result += parentRect.bottomLeft();
    }

    return result;
}
