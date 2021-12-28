/*
 * Photo Broom - photos management tool.
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

#include <QLocale>

#include <core/base_tags.hpp>
#include <core/ilogger_factory.hpp>
#include <core/ilogger.hpp>
#include <database/database_tools/itag_info_collector.hpp>

#include "utils/variant_display.hpp"


namespace
{
    template<typename T>
    QString limited_join(T first, T last, int limit, const QString& splitter)
    {
        QString result;

        for(; limit > 0 && first != last;)
        {
            const TagValue& v = *first++;
            result += v.rawValue();

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


TagValueModel::TagValueModel(const std::set<Tag::Types>& infos, ITagInfoCollector* collector, ILoggerFactory* loggerFactory):
    m_values(),
    m_tagInfos(infos),
    m_tagInfoCollector(collector),
    m_loggerFactory(loggerFactory)
{
    connect(m_tagInfoCollector, &ITagInfoCollector::setOfValuesChanged,
            this,               &TagValueModel::collectorNotification);

    updateData();
}


TagValueModel::~TagValueModel()
{

}


const QStringList& TagValueModel::data() const
{
    return m_values;
}


void TagValueModel::updateData()
{
    m_values.clear();

    QString combined_name;

    for(const Tag::Types& info: m_tagInfos)
    {
        const auto& values = m_tagInfoCollector->get(info);
        std::transform( values.begin(), values.end(), std::back_inserter(m_values), [](const TagValue& value){
            const QLocale locale;

            return localize(value.get(), locale);
        });

        combined_name += combined_name.isEmpty()?
            BaseTags::getName(info) :
            ", " + BaseTags::getName(info);
    }

    const QString values_joined = limited_join(m_values.begin(), m_values.end(), 10, ", ");
    const QString logMessage = QString("Got %1 values for %2: %3")
                                    .arg(m_values.size())
                                    .arg(combined_name)
                                    .arg(values_joined);

    auto logger = m_loggerFactory->get({"gui", "TagValueModel"});
    logger->debug(logMessage);

    emit dataChanged();
}


void TagValueModel::collectorNotification(const Tag::Types& tagType)
{
    if ( m_tagInfos.find(tagType) != m_tagInfos.end())
        updateData();
}
