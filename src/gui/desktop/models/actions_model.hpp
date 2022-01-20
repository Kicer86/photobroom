
#ifndef ACTIONSMODEL_HPP
#define ACTIONSMODEL_HPP

#include <QAbstractListModel>
#include <QAction>

#include "roles_expansion.hpp"


class ActionsModel: public RolesExpansion<QAbstractListModel>
{
public:
    ActionsModel();

    QVariant data(const QModelIndex & index, int role) const override;
    int rowCount(const QModelIndex & parent) const override;

    void addActions(std::vector<std::unique_ptr<QAction>> &&);
    void clear();

private:
    std::vector<std::unique_ptr<QAction>> m_actions;
};

#endif
