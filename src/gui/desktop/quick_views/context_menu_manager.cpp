
#include "context_menu_manager.hpp"


ContextMenuManager::ContextMenuManager()
{

}


const QAbstractItemModel* ContextMenuManager::model() const
{
    return &m_model;
}
