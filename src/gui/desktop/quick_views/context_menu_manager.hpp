
#ifndef CONTEXT_MENU_MANAGER_HPP_INCLUDED
#define CONTEXT_MENU_MANAGER_HPP_INCLUDED


#include <QObject>
#include "models/actions_model.hpp"


class ContextMenuManager: public QObject
{
    Q_OBJECT

    Q_PROPERTY(const QAbstractItemModel* model READ model CONSTANT)

public:
    ContextMenuManager();

    const QAbstractItemModel* model() const;

private:
    ActionsModel m_model;
};

#endif
