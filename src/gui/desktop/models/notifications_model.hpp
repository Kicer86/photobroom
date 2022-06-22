
#ifndef NOTIFICATIONSMODEL_HPP
#define NOTIFICATIONSMODEL_HPP

#include <QAbstractListModel>

#include "inotifications.hpp"


class NotificationsModel: public QAbstractListModel, public INotifications
{
    Q_OBJECT
    Q_PROPERTY(int count READ getCount() NOTIFY countChanged)

public:
    int insert(const QString &, Type) override;
    Q_INVOKABLE void removeRow(int row);
    void removeId(int id) override;

    int getCount() const;

    int rowCount(const QModelIndex & parent) const override;
    QVariant data(const QModelIndex & index, int role) const override;

signals:
    void countChanged(int);

private:
    std::vector<std::pair<int, QString>> m_data;
    int m_id = 0;
};

#endif
