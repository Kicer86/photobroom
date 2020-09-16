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

#include <chrono>

#include <core/function_wrappers.hpp>
#include <core/imodel_compositor_data_source.hpp>
#include <database/iphoto_operator.hpp>
#include "models/flat_model.hpp"
#include "photos_model_controller_component.hpp"
#include "ui_utils/icompleter_factory.hpp"


using namespace std::placeholders;
using namespace std::chrono_literals;

namespace
{
    const char* expressions_separator = ",";
}


PhotosModelControllerComponent::PhotosModelControllerComponent(QObject* p)
    : QObject(p)
    , m_model(new FlatModel(this))
    , m_db(nullptr)
    , m_completerFactory(nullptr)
    , m_newPhotosOnly(false)
{
    m_searchLauncher.setSingleShot(true);
    connect(&m_searchLauncher, &QTimer::timeout, this, &PhotosModelControllerComponent::updateModelFilters);
}


void PhotosModelControllerComponent::setDatabase(Database::IDatabase* db)
{
    m_db = db;
    m_model->setDatabase(db);

    if (m_db != nullptr)
        updateTimeRange();
}


void PhotosModelControllerComponent::setCompleterFactory(ICompleterFactory* completerFactory)
{
    m_completerFactory = completerFactory;

    auto categoriesModel = completerFactory->accessModel(TagTypes::Category);
    connect(categoriesModel, &IModelCompositorDataSource::dataChanged,
            this, &PhotosModelControllerComponent::categoriesChanged);

    emit categoriesChanged();
}


APhotoInfoModel* PhotosModelControllerComponent::model() const
{
    return m_model;
}


QStringList PhotosModelControllerComponent::categories() const
{
    QStringList values;

    if (m_completerFactory)
    {
        const auto rawValues = rawCategories();

        values.reserve(rawValues.size() + 1);
        values.push_back(tr("All"));

        for(const QString& rawValue: rawValues)
        {
            const QRgba64 rgba = QRgba64::fromRgba64(rawValue.toULongLong());
            const QString formattedValue = QString("#%1%2%3")
                                              .arg(rgba.red8(), 2, 16, QChar('0'))
                                              .arg(rgba.green8(), 2, 16, QChar('0'))
                                              .arg(rgba.blue8(), 2, 16, QChar('0'));

            values.push_back(formattedValue);
        }
    }

    return values;
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


QString PhotosModelControllerComponent::searchExpression() const
{
    return m_searchExpression;
}


bool PhotosModelControllerComponent::newPhotosOnly() const
{
    return m_newPhotosOnly;
}


int PhotosModelControllerComponent::category() const
{
    if (m_categoryFilter.isEmpty())
        return 0;
    else
    {
        const auto cats = rawCategories();
        auto it = std::find(cats.cbegin(), cats.cend(), m_categoryFilter);

        assert(it != cats.end());
        return std::distance(cats.cbegin(), it) + 1;
    }
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


void PhotosModelControllerComponent::setSearchExpression(const QString& expression)
{
    const bool differ = expression != m_searchExpression;
    m_searchExpression = expression;

    if (differ)
        m_searchLauncher.start(1s);
}


void PhotosModelControllerComponent::setNewPhotosOnly(bool v)
{
    m_newPhotosOnly = v;

    updateModelFilters();
}


void PhotosModelControllerComponent::setCategory(int category)
{
    if (category <= 0)
        m_categoryFilter.clear();
    else
    {
        const auto cats = rawCategories();
        assert(cats.size() >= category);

        m_categoryFilter = cats[category - 1];
    }

    updateModelFilters();
}


QDate PhotosModelControllerComponent::dateFor(unsigned int idx) const
{
    return idx >= m_dates.size()? QDate(): m_dates[idx];
}


void PhotosModelControllerComponent::markNewAsReviewed()
{
    m_db->exec(std::bind(&PhotosModelControllerComponent::markPhotosAsReviewed, this, _1));
}


void PhotosModelControllerComponent::updateModelFilters()
{
    auto filters_for_model = allFilters();

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


std::vector<Database::IFilter::Ptr> PhotosModelControllerComponent::allFilters() const
{
    std::vector<Database::IFilter::Ptr> filters_for_model;

    if (m_dates.empty() == false)
    {
        const QDate from = m_dates[m_timeView.first];
        const QDate to = m_dates[m_timeView.second];

        filters_for_model.push_back( std::make_shared<Database::FilterPhotosWithTag>(TagTypes::Date, from, Database::FilterPhotosWithTag::ValueMode::GreaterOrEqual, true) );
        filters_for_model.push_back( std::make_shared<Database::FilterPhotosWithTag>(TagTypes::Date, to, Database::FilterPhotosWithTag::ValueMode::LessOrEqual, true) );
    }

    const SearchExpressionEvaluator::Expression expression = SearchExpressionEvaluator(expressions_separator).evaluate(m_searchExpression);

    if (expression.empty() == false)
        filters_for_model.push_back( std::make_shared<Database::FilterPhotosMatchingExpression>(expression) );

    if (m_newPhotosOnly)
    {
        const std::map flags = { std::pair{Photo::FlagsE::StagingArea, 1} };
        filters_for_model.push_back( std::make_shared<Database::FilterPhotosWithFlags>(flags) );
    }

    if (m_categoryFilter.isEmpty() == false)
    {
        auto categoryFitler = std::make_shared<Database::FilterPhotosWithTag>(TagTypes::Category, m_categoryFilter);
        filters_for_model.push_back(categoryFitler);
    }

    return filters_for_model;
}


QStringList PhotosModelControllerComponent::rawCategories() const
{
    if (m_completerFactory)
    {
        auto categoriesModel = m_completerFactory->accessModel(TagTypes::Category);
        const auto rawValues = categoriesModel->data();

        return rawValues;
    }
    else
        return {};
}


void PhotosModelControllerComponent::getTimeRangeForFilters(Database::IBackend& backend)
{
    auto dates = backend.listTagValues(TagTypes::Date, {});

    const auto with_date_filter = std::make_shared<Database::FilterPhotosWithTag>(TagTypes::Date);
    const auto without_date_filter = std::make_shared<Database::FilterNotMatchingFilter>(with_date_filter);
    const std::vector<Database::IFilter::Ptr> photos_without_date_tag_filter = { without_date_filter };
    const auto photos_without_date_tag = backend.photoOperator().getPhotos(photos_without_date_tag_filter);

    if (photos_without_date_tag.empty() == false)
        dates.push_back(QDate());

    std::sort(dates.begin(), dates.end());
    invokeMethod(this, &PhotosModelControllerComponent::setAvailableDates, dates);
}


void PhotosModelControllerComponent::markPhotosAsReviewed(Database::IBackend& backend)
{
    backend.markStagedAsReviewed();
    invokeMethod(this, &PhotosModelControllerComponent::updateModelFilters);
}
