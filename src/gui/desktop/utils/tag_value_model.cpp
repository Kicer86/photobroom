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

#include <core/cross_thread_callback.hpp>
#include <core/ilogger_factory.hpp>
#include <core/ilogger.hpp>
#include <database/database_tools/itag_info_collector.hpp>


namespace
{
    template<typename T>
    QString limited_join(T first, T last, int limit, const QString& splitter)
    {
        QString result;

        for(; limit > 0 && first != last;)
        {
            const TagValue& v = *first++;
            result += v.formattedValue();

            limit--;

            if (first != last)
            {
                result += splitter;

                if (limit == 0)
                    result += "...";
            }
        }

        return result;
    }
}


TagValueModel::TagValueModel(const TagNameInfo& info):
    m_values(),
    m_tagInfo(info),
    m_tagInfoCollector(nullptr),
    m_loggerFactory(nullptr),
    m_observerId(0)
{

}


TagValueModel::~TagValueModel()
{
    if (m_tagInfoCollector)
        m_tagInfoCollector->unregisterChangeObserver(m_observerId);
}


void TagValueModel::set(ITagInfoCollector* collector)
{
    m_tagInfoCollector = collector;

    using namespace std::placeholders;
    std::function<void(const TagNameInfo &)> callback = std::bind(&TagValueModel::collectorNotification, this, _1);
    auto mainThreadCallback = cross_thread_function(this, callback);                  // make sure we won't have problems with calls from other threads

    m_observerId = m_tagInfoCollector->registerChangeObserver(mainThreadCallback);

    updateData();
}


void TagValueModel::set(ILoggerFactory* loggerFactory)
{
    m_loggerFactory = loggerFactory;
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
    assert(static_cast<std::size_t>(index.row()) < m_values.size());

    QVariant result;

    if (role == Qt::EditRole || role == Qt::DisplayRole)
        result = m_values[index.row()].get();

    return result;
}


void TagValueModel::updateData()
{
    beginResetModel();

    const auto& values = m_tagInfoCollector->get(m_tagInfo);

    m_values.clear();
    std::copy( values.begin(), values.end(), std::back_inserter(m_values) );

    const QString values_joined = limited_join(m_values.begin(), m_values.end(), 10, ", ");
    const QString logMessage = QString("Got %1 values for %2: %3")
                                    .arg(m_values.size())
                                    .arg(m_tagInfo.getName())
                                    .arg(values_joined);

    auto logger = m_loggerFactory->get({"gui", "TagValueModel"});
    DebugStream(logger.get()) << logMessage;

    endResetModel();
}


void TagValueModel::collectorNotification(const TagNameInfo& tagInfo)
{
    if (m_tagInfo == tagInfo)
        updateData();
}
