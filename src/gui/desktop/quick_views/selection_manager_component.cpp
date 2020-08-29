
#include "selection_manager_component.hpp"


SelectionManagerComponent::SelectionManagerComponent(QObject* p)
    : QObject(p)
{

}


void SelectionManagerComponent::toggleIndexSelection(int index)
{
    auto it = m_selected.find(index);

    if (it == m_selected.end())
        m_selected.insert(index);
    else
        m_selected.erase(it);
}


void SelectionManagerComponent::clearSelection()
{
    m_selected.clear();
}


bool SelectionManagerComponent::isIndexSelected(int index) const
{
    auto it = m_selected.find(index);

    return it != m_selected.end();
}


QList<int> SelectionManagerComponent::selected() const
{
    return { m_selected.begin(), m_selected.end() };
}
