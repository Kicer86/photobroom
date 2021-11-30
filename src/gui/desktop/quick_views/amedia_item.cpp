
#include "amedia_item.hpp"


AMediaItem::AMediaItem(QQuickItem* p)
    : QQuickPaintedItem(p)
    , m_state(State::NotFetched)
{

}


AMediaItem::State AMediaItem::state() const
{
    return m_state;
}


void AMediaItem::setState(AMediaItem::State state)
{
    const bool changed = state != m_state;
    m_state = state;

    if (changed)
        emit stateChanged();
}
