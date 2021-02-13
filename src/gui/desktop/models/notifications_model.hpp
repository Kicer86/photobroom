
#ifndef NOTIFICATIONSMODEL_HPP
#define NOTIFICATIONSMODEL_HPP

#include <QAbstractListModel>

class NotificationsModel: public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ getCount() NOTIFY countChanged);

public:
    int insertWarning(const QString &);
    Q_INVOKABLE void removeWarning(int row);
    void removeWarningWithId(int id);

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
