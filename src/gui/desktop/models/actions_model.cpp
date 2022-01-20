
#include "actions_model.hpp"


namespace
{
    enum Roles
    {
        ActionName = Qt::UserRole + 1,
        ActionEnabled,
    };
}


ActionsModel::ActionsModel()
{
    registerRole(ActionName, "actionName");
    registerRole(ActionEnabled, "actionEnabled");
}


QVariant ActionsModel::data(const QModelIndex& index, int role) const
{
    QVariant result;
    const int c = index.column();
    const unsigned r = static_cast<unsigned>(index.row());

    if (c == 0 && r < m_actions.size())
    {
        auto& action = m_actions[r];

        switch(role)
        {
            case ActionName:
                result = action->text();
                break;

            case ActionEnabled:
                result = action->isEnabled();
                break;

            default:
                break;
        }
    }

    return result;
}


int ActionsModel::rowCount(const QModelIndex& parent) const
{
    return parent.isValid()? 0: static_cast<int>(m_actions.size());
}


void ActionsModel::addActions(std::vector<std::unique_ptr<QAction>>&& actions)
{
    const int items = static_cast<int>(m_actions.size());
    const int new_items = static_cast<int>(actions.size());
    beginInsertRows({}, items, items + new_items - 1);

    m_actions.insert(m_actions.end(), std::make_move_iterator(actions.begin()), std::make_move_iterator(actions.end()));

    endInsertRows();
}


void ActionsModel::clear()
{
    const int items = static_cast<int>(m_actions.size());
    beginRemoveRows({}, 0, items - 1);

    m_actions.clear();

    endRemoveRows();
}
