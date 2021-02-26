
#include "properties_controlled_model.hpp"

PropertiesControlledModel::PropertiesControlledModel(QObject* parent)
    : QObject(parent)
{

}


QAbstractItemModel* PropertiesControlledModel::model()
{
    return &m_model;
}


Database::IDatabase* PropertiesControlledModel::database() const
{
    return m_model.database();
}


void PropertiesControlledModel::setDatabase(Database::IDatabase* db)
{
    m_model.setDatabase(db);
}


QVariantMap PropertiesControlledModel::generalFlags() const
{
    return m_generalFlags;
}


QVariantMap PropertiesControlledModel::tags() const
{
    return m_tags;
}


void PropertiesControlledModel::setGeneralFlags(const QVariantMap& generalFlags)
{
    m_generalFlags = generalFlags;

    emit generalFlagsChanged(m_generalFlags);

    updateFilters();
}


void PropertiesControlledModel::setTags(const QVariantMap& tags)
{
    m_tags = tags;

    emit generalFlagsChanged(m_tags);

    updateFilters();
}


void PropertiesControlledModel::updateFilters()
{
    Database::Filter filter = tagsFilter();

    m_model.setFilter(filter);
}


Database::Filter PropertiesControlledModel::tagsFilter()
{
    std::vector<Database::Filter> filters;

    for (auto it = m_tags.begin(); it != m_tags.end(); ++it)
    {
        if (it.key() == "Date")
            filters.push_back(Database::FilterPhotosWithTag(TagTypes::Date, TagValue::fromQVariant(it.value()),
                                                            Database::FilterPhotosWithTag::ValueMode::Equal, true));
    }

    return Database::GroupFilter(filters);
}
