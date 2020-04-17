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

#include "photos_model_controller_component.hpp"

#include <core/function_wrappers.hpp>
#include <database/iphoto_operator.hpp>
#include "models/flat_model.hpp"


using namespace std::placeholders;


PhotosModelControllerComponent::PhotosModelControllerComponent(QObject* p)
    : QObject(p)
    , m_model(new FlatModel(this))
{
}


void PhotosModelControllerComponent::setDatabase(Database::IDatabase* db)
{
    m_db = db;
    m_model->setDatabase(db);

    if (m_db != nullptr)
        updateTimeRange();
}


QAbstractItemModel* PhotosModelControllerComponent::model() const
{
    return m_model;
}


unsigned int PhotosModelControllerComponent::datesCount() const
{
    return static_cast<unsigned int>(m_dates.size());
}


unsigned int PhotosModelControllerComponent::timeViewFrom() const
{
    return m_timeView.first;
}


unsigned int PhotosModelControllerComponent::timeViewTo() const
{
    return m_timeView.second;
}


void PhotosModelControllerComponent::setTimeViewFrom(unsigned int viewFrom)
{
    m_timeView.first = viewFrom;

    updateModelFilters();
}


void PhotosModelControllerComponent::setTimeViewTo(unsigned int viewTo)
{
    m_timeView.second = viewTo;

    updateModelFilters();
}


QDate PhotosModelControllerComponent::dateFor(unsigned int idx) const
{
    return idx >= m_dates.size()? QDate(): m_dates[idx];
}


void PhotosModelControllerComponent::updateModelFilters()
{
    auto filters_for_model = filters();
    const QDate from = m_dates[m_timeView.first];
    const QDate to = m_dates[m_timeView.second];

    filters_for_model.push_back( std::make_shared<Database::FilterPhotosWithTag>(TagTypes::Date, from, Database::FilterPhotosWithTag::ValueMode::GreaterOrEqual, true) );
    filters_for_model.push_back( std::make_shared<Database::FilterPhotosWithTag>(TagTypes::Date, to, Database::FilterPhotosWithTag::ValueMode::LessOrEqual, true) );

    m_model->setFilters(filters_for_model);
}


void PhotosModelControllerComponent::setAvailableDates(const std::vector<TagValue>& dates_as_tags)
{
    std::vector<QDate> dates;
    std::transform(dates_as_tags.begin(), dates_as_tags.end(), std::back_inserter(dates), [](const auto& d) { return d.getDate(); });

    if (dates != m_dates)
    {
        m_dates = dates;

        const unsigned int count = static_cast<unsigned int>(dates.size());
        const unsigned int timeViewTo = count == 0? 0: count - 1;

        m_timeView = { 0, timeViewTo };

        emit datesCountChanged();
    }
}


void PhotosModelControllerComponent::updateTimeRange()
{
    m_db->exec(std::bind(&PhotosModelControllerComponent::getTimeRangeForFilters, this, _1));
}


std::vector<Database::IFilter::Ptr> PhotosModelControllerComponent::filters() const
{
    return m_filters;
}


void PhotosModelControllerComponent::getTimeRangeForFilters(Database::IBackend* backend)
{
    const auto range_filters = filters();
    auto dates = backend->listTagValues(TagTypes::Date, range_filters);

    const auto with_date_filter = std::make_shared<Database::FilterPhotosWithTag>(TagTypes::Date);
    const auto without_date_filter = std::make_shared<Database::FilterNotMatchingFilter>(with_date_filter);
    const std::vector<Database::IFilter::Ptr> photos_without_date_tag_filter = { without_date_filter };
    const auto photos_without_date_tag = backend->photoOperator().getPhotos(photos_without_date_tag_filter);

    if (photos_without_date_tag.empty() == false)
        dates.push_back(QDate());

    std::sort(dates.begin(), dates.end());
    invokeMethod(this, &PhotosModelControllerComponent::setAvailableDates, dates);
}
