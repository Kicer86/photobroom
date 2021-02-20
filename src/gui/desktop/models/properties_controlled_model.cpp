
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
