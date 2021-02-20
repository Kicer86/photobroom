
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
