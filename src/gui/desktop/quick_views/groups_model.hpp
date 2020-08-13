
#ifndef GROUPSMODEL_HPP
#define GROUPSMODEL_HPP

#include <QAbstractItemModel>


class GroupsModel: public QAbstractListModel
{
public:
    QVariant data(const QModelIndex& index, int role) const override;
    int columnCount(const QModelIndex& parent) const override;
    int rowCount(const QModelIndex& parent) const override;
    QModelIndex parent(const QModelIndex& child) const override;
};

#endif // GROUPSMODEL_HPP
