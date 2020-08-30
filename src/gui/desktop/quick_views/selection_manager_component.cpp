
#include "selection_manager_component.hpp"


SelectionManagerComponent::SelectionManagerComponent(QObject* p)
    : QObject(p)
{

}


void SelectionManagerComponent::toggleIndexSelection(int index)
{
    const auto previous = m_selected;

    auto it = m_selected.find(index);

    if (it == m_selected.end())
        m_selected.insert(index);
    else
        m_selected.erase(it);

    calculateChange(previous, m_selected);
}


void SelectionManagerComponent::clearSelection()
{
    const auto previous = m_selected;

    m_selected.clear();

    calculateChange(previous, m_selected);
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


void SelectionManagerComponent::calculateChange(const std::set<int>& previous, const std::set<int>& current)
{
    QList<int> unselected_items;
    QList<int> selected_items;

    std::set_difference(previous.begin(), previous.end(),
                        current.begin(), current.end(),
                        std::back_inserter(unselected_items));

    std::set_difference(current.begin(), current.end(),
                        previous.begin(), previous.end(),
                        std::back_inserter(selected_items));

    emit selectionChanged(unselected_items, selected_items);
}
