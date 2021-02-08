
#ifndef NOTIFICATIONSMODEL_HPP
#define NOTIFICATIONSMODEL_HPP

#include <QAbstractListModel>

class NotificationsModel: public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ getCount() NOTIFY countChanged);

public:
    void insertWarning(const QString &);

    int getCount() const;

    int rowCount(const QModelIndex & parent) const override;
    QVariant data(const QModelIndex & index, int role) const override;

signals:
    void countChanged(int);

private:
    QStringList m_data;

};

#endif
