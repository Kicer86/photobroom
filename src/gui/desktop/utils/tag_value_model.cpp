/*
 * Model for particular TagNameInfo values
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

#include "tag_value_model.hpp"

#include <database/database_tools/itag_info_collector.hpp>


TagValueModel::TagValueModel(const TagNameInfo& info):
    m_values(),
    m_tagInfo(info),
    m_tagInfoCollector(nullptr)
{

}


TagValueModel::~TagValueModel()
{

}


void TagValueModel::set(ITagInfoCollector* collector)
{
    m_tagInfoCollector = collector;

    const auto& values = collector->get(m_tagInfo);

    std::copy( values.begin(), values.end(), std::back_inserter(m_values) );
}


int TagValueModel::rowCount(const QModelIndex& index) const
{
    const int result = index.isValid()? 0: m_values.size();

    return result;
}


QVariant TagValueModel::data(const QModelIndex& index, int role) const
{
    assert(index.isValid());
    assert(index.column() == 0);
    assert(index.row() < m_values.size());

    QVariant result;

    if (role == Qt::EditRole || role == Qt::DisplayRole)
        result = m_values[index.row()].get();

    return result;
}
