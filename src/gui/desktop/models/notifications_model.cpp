
#include "notifications_model.hpp"

int NotificationsModel::insertWarning(const QString& warning)
{
    const int id = m_id;
    m_id++;

    const int count = getCount();

    beginInsertRows({}, count, count);
    m_data.emplace_back(id, warning);
    endInsertRows();

    emit countChanged(getCount());

    return id;
}


void NotificationsModel::removeWarning(int row)
{
    beginRemoveRows({}, row, row);
    m_data.erase(m_data.begin() + row);
    endRemoveRows();

    emit countChanged(getCount());
}


int NotificationsModel::getCount() const
{
    return m_data.size();
}


int NotificationsModel::rowCount(const QModelIndex& parent) const
{
    return parent.isValid()? 0: getCount();
}

QVariant NotificationsModel::data(const QModelIndex& index, int role) const
{
    return role == Qt::DisplayRole? m_data[index.row()].second: QVariant();
}
