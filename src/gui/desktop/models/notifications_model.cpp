
#include "notifications_model.hpp"

INotifications::Id NotificationsModel::insert(const QString& warning, Type type)
{
    const Id id = m_id;
    m_id.next();

    const int count = getCount();

    beginInsertRows({}, count, count);
    m_data.emplace_back(id, warning, type);
    endInsertRows();

    emit countChanged(getCount());

    return id;
}


void NotificationsModel::removeRow(int row)
{
    beginRemoveRows({}, row, row);
    m_data.erase(m_data.begin() + row);
    endRemoveRows();

    emit countChanged(getCount());
}


void NotificationsModel::remove(INotifications::Id id)
{
    const auto it = std::find_if(m_data.begin(), m_data.end(), [id](const auto& item) {
        return std::get<0>(item) == id;
    });

    if (it != m_data.end())
    {
        const auto pos = std::distance(m_data.begin(), it);
        removeRow(static_cast<int>(pos));
    }
}


int NotificationsModel::getCount() const
{
    return static_cast<int>(m_data.size());
}


int NotificationsModel::rowCount(const QModelIndex& parent) const
{
    return parent.isValid()? 0: getCount();
}

QVariant NotificationsModel::data(const QModelIndex& index, int role) const
{
    return role == Qt::DisplayRole? std::get<1>(m_data[index.row()]): QVariant();
}
