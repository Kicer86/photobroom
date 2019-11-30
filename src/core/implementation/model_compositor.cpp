/*
 * Copyright (C) 2019  Michał Walenciak <Kicer86@gmail.com>
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
 */

#include "model_compositor.hpp"

#include <numeric>

void ModelCompositor::add(IModelCompositorDataSource* dataSource)
{
    m_sources.push_back(dataSource);
}


int ModelCompositor::rowCount(const QModelIndex& parent) const
{
    assert(parent.isValid() == false);

    const auto count = std::accumulate(m_sources.begin(), m_sources.end(), 0, [](const auto& c, const auto& i)
    {
        return c + std::size(i->data());
    });

    return count;
}


QVariant ModelCompositor::data(const QModelIndex& idx, int) const
{
    assert(idx.isValid());
    assert(idx.column() == 0);

    QVariant result;

    int row_in_data_source = idx.row();

    for (IModelCompositorDataSource* data_source: m_sources)
    {
        const auto data_source_size = data_source->data().size();

        if (row_in_data_source < data_source_size)
        {
            result = data_source->data().at(row_in_data_source);
            break;
        }
        else
            row_in_data_source -= data_source_size;
    }

    return result;
}
