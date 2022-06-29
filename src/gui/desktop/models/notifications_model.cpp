
#include <core/qmodel_utils.hpp>
#include "notifications_model.hpp"


ENUM_ROLES_SETUP(NotificationsModel::Roles);


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
    QVariant result;

    switch(role)
    {
        case Qt::DisplayRole: result = std::get<1>(m_data[index.row()]); break;
        case TypeRole:        result = static_cast<int>(std::get<2>(m_data[index.row()])); break;
    }

    return result;
}


QHash<int, QByteArray> NotificationsModel::roleNames() const
{
    auto roles = QAbstractListModel::roleNames();
    const auto extra = parseRoles<Roles>();
    const QHash<int, QByteArray> extraRoles(extra.begin(), extra.end());
    roles.insert(extraRoles);

    return roles;
}
