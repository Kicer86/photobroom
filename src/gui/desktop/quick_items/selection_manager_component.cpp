
#include "selection_manager_component.hpp"


SelectionManagerComponent::SelectionManagerComponent(QObject* p)
    : QObject(p)
    , m_previouslySelected(-1)
{

}


void SelectionManagerComponent::clearAndToggleIndexSelection(int index)
{
    const auto previous = m_selected;

    clear();
    toggle(index);

    calculateChange(previous, m_selected);
}


void SelectionManagerComponent::toggleIndexSelection(int index)
{
    const auto previous = m_selected;

    toggle(index);

    calculateChange(previous, m_selected);
}


void SelectionManagerComponent::clearSelection()
{
    const auto previous = m_selected;

    clear();

    calculateChange(previous, m_selected);
}


void SelectionManagerComponent::selectTo(int index)
{
    const auto previous = m_selected;

    if (m_previouslySelected == -1)
        toggleIndexSelection(index);
    else
    {
        m_selected.clear();

        const int from = std::min(m_previouslySelected, index);
        const int to = std::max(m_previouslySelected, index);

        for(int i = from; i <= to; i++)
            m_selected.insert(i);
    }

    calculateChange(previous, m_selected);
}


bool SelectionManagerComponent::isIndexSelected(int index) const
{
    auto it = m_selected.find(index);

    return it != m_selected.end();
}


QList<int> SelectionManagerComponent::selected() const
{
    return QList<int>(m_selected.begin(), m_selected.end());
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
    emit selectedChanged( {current.begin(), current.end()} );
}


void SelectionManagerComponent::toggle(int index)
{
    if (index >= 0)
    {
        auto it = m_selected.find(index);

        if (it == m_selected.end())
        {
            m_selected.insert(index);
            m_previouslySelected = index;
        }
        else
            m_selected.erase(it);
    }
    else
        m_selected.clear();
}


void SelectionManagerComponent::clear()
{
    m_previouslySelected = -1;
    m_selected.clear();
}
