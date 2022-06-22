
#include "notifications_model.hpp"

int NotificationsModel::insert(const QString& warning, Type type)
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


void NotificationsModel::removeRow(int row)
{
    beginRemoveRows({}, row, row);
    m_data.erase(m_data.begin() + row);
    endRemoveRows();

    emit countChanged(getCount());
}


void NotificationsModel::removeId(int id)
{
    const auto it = std::find_if(m_data.begin(), m_data.end(), [id](const auto item) {
        return item.first == id;
    });

    if (it != m_data.end())
    {
        const int pos = std::distance(m_data.begin(), it);
        removeRow(pos);
    }
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
