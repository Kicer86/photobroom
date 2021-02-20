
#include "properties_controlled_model.hpp"

PropertiesControlledModel::PropertiesControlledModel(QObject* parent)
    : QAbstractListModel(parent)
{

}


QVariant PropertiesControlledModel::data(const QModelIndex& index, int role) const
{
    return m_model.data(index, role);
}


int PropertiesControlledModel::rowCount(const QModelIndex& parent) const
{
    return m_model.rowCount(parent);
}


Database::IDatabase* PropertiesControlledModel::database() const
{
    return m_model.database();
}


void PropertiesControlledModel::setDatabase(Database::IDatabase* db)
{
    m_model.setDatabase(db);
}
