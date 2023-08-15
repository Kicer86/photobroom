/*
 * Photo Broom - photos management tool.
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



ModelCompositor::ModelCompositor(QObject* parent)
    : QAbstractListModel(parent)
{
}


ModelCompositor::~ModelCompositor()
{
    for(auto& connection: m_connections)
        QObject::disconnect(connection);
}


void ModelCompositor::add(const IModelCompositorDataSource* dataSource)
{
    const auto current_size = rowCount();
    const bool is_data_source_empty = dataSource->data().empty();

    if (is_data_source_empty == false)
        beginInsertRows(QModelIndex(), current_size, current_size + static_cast<int>(dataSource->data().size()) - 1);

    m_sources.emplace(dataSource, dataSource->data().size());

    if (is_data_source_empty == false)
        endInsertRows();

    auto connection = connect(dataSource, &IModelCompositorDataSource::dataChanged,
                              std::bind(&ModelCompositor::dataSourceChanged, this, dataSource));

    m_connections.push_back(connection);
}


int ModelCompositor::rowCount(const QModelIndex& parent) const
{
    assert(parent.isValid() == false);

    const auto count = std::accumulate(m_sources.begin(), m_sources.end(), 0, [](const auto& c, const auto& i)
    {
        return c + i.second;
    });

    return count;
}


QVariant ModelCompositor::data(const QModelIndex& idx, int role) const
{
    assert(idx.isValid());
    assert(idx.column() == 0);

    QVariant result;

    if (role == Qt::EditRole || role == Qt::DisplayRole)
    {
        int row_in_data_source = idx.row();

        for (const auto& [data_source, data_source_size]: m_sources)
        {
            if (row_in_data_source < data_source_size)
            {
                result = data_source->data().at(row_in_data_source);
                break;
            }
            else
                row_in_data_source -= data_source_size;
        }
    }

    return result;
}


void ModelCompositor::dataSourceChanged(const IModelCompositorDataSource* changed_source)
{
    int begin = 0;
    for (auto& [data_source, data_source_size]: m_sources)
    {
        if (data_source == changed_source)
        {
            if (data_source_size > 0)
            {
                beginRemoveRows(QModelIndex(), begin, begin + data_source_size - 1);
                data_source_size = 0;
                endRemoveRows();
            }

            const auto changed_source_size = static_cast<int>(changed_source->data().size());

            if (changed_source_size > 0)
            {
                beginInsertRows(QModelIndex(), begin, begin + changed_source_size - 1);
                data_source_size = changed_source_size;
                endInsertRows();
            }

            break;
        }
        else
            begin += data_source_size;
    }
}
