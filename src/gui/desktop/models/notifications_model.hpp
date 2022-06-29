
#ifndef NOTIFICATIONSMODEL_HPP
#define NOTIFICATIONSMODEL_HPP

#include <QAbstractListModel>

#include "inotifications.hpp"


class NotificationsModel: public QAbstractListModel, public INotifications
{
    Q_OBJECT
    Q_PROPERTY(int count READ getCount() NOTIFY countChanged)

public:
    enum Roles
    {
        TypeRole = Qt::UserRole + 1,
    };

    Id insert(const QString &, Type) override;
    Q_INVOKABLE void removeRow(int row);
    void remove(Id id) override;

    int getCount() const;

    int rowCount(const QModelIndex & parent) const override;
    QVariant data(const QModelIndex & index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

signals:
    void countChanged(int);

private:
    std::vector<std::tuple<Id, QString, Type>> m_data;
    Id m_id = Id(0);
};

#endif
