/*
 * Copyright (C) 2020  Michał Walenciak <Kicer86@gmail.com>
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

#include "photos_model_component.hpp"

#include <core/function_wrappers.hpp>
#include "models/flat_model.hpp"


using namespace std::placeholders;

PhotosModelComponent::PhotosModelComponent(QObject* p)
    : QObject(p)
    , m_model(new FlatModel(this))
{
}


void PhotosModelComponent::setDatabase(Database::IDatabase* db)
{
    m_db = db;
    m_model->setDatabase(db);

    if (m_db != nullptr)
        updateTimeRange();
}


QAbstractItemModel* PhotosModelComponent::model() const
{
    return m_model;
}


const QDate& PhotosModelComponent::timeRangeFrom() const
{
    return m_timeRange.first;
}


const QDate& PhotosModelComponent::timeRangeTo() const
{
    return m_timeRange.second;
}


const QDate& PhotosModelComponent::timeViewFrom() const
{
    return m_timeView.first;
}


const QDate& PhotosModelComponent::timeViewTo() const
{
    return m_timeView.second;
}


void PhotosModelComponent::setTimeViewFrom(const QDate& viewFrom)
{
    m_timeView.first = viewFrom;

    updateModelFilters();
}


void PhotosModelComponent::setTimeViewTo(const QDate& viewTo)
{
    m_timeView.second = viewTo;

    updateModelFilters();
}


void PhotosModelComponent::updateModelFilters()
{
    auto filters_for_model = filters();
    filters_for_model.push_back( std::make_shared<Database::FilterPhotosWithTag>(TagTypes::Date, m_timeView.first, Database::FilterPhotosWithTag::ValueMode::GreaterOrEqual) );
    filters_for_model.push_back( std::make_shared<Database::FilterPhotosWithTag>(TagTypes::Date, m_timeView.second, Database::FilterPhotosWithTag::ValueMode::LessOrEqual) );

    m_model->setFilters(filters_for_model);
}


void PhotosModelComponent::setTimeRange(const QDate& from, const QDate& to)
{
    const QPair newTimeRange(from, to);

    if (newTimeRange != m_timeRange)
    {
        m_timeRange = QPair(from, to);
        m_timeView = m_timeRange;

        emit timeRangeFromChanged();
        emit timeRangeToChanged();
    }
}


void PhotosModelComponent::updateTimeRange()
{
    m_db->exec(std::bind(&PhotosModelComponent::getTimeRangeForFilters, this, _1));
}


std::vector<Database::IFilter::Ptr> PhotosModelComponent::filters() const
{
    return m_filters;
}


void PhotosModelComponent::getTimeRangeForFilters(Database::IBackend* backend)
{
    const auto range_filters = filters();
    const auto dates = backend->listTagValues(TagTypes::Date, range_filters);

    if (dates.empty() == false)
    {
        const auto dates_range = std::minmax_element(dates.begin(), dates.end());

        invokeMethod(this, &PhotosModelComponent::setTimeRange, dates_range.first->getDate(), dates_range.second->getDate());
    }
    else
        invokeMethod(this, &PhotosModelComponent::setTimeRange, QDate(), QDate());
}
