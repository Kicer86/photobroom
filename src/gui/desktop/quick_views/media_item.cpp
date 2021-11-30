
#include "media_item.hpp"


MediaItem::MediaItem(QQuickItem* p)
    : QQuickPaintedItem(p)
    , m_state(State::NotFetched)
{

}


MediaItem::State MediaItem::state() const
{
    return m_state;
}


void MediaItem::setState(MediaItem::State state)
{
    const bool changed = state != m_state;
    m_state = state;

    if (changed)
        emit stateChanged();
}
