
#include "amedia_item.hpp"


AMediaItem::AMediaItem(QQuickItem* p)
    : QQuickPaintedItem(p)
    , m_state(State::NotFetched)
{

}


void AMediaItem::setSource(const Photo::Id& id)
{
    m_id = id;
    emit sourceChanged(id);

    update();
}


AMediaItem::State AMediaItem::state() const
{
    return m_state;
}


const Photo::Id& AMediaItem::source() const
{
    return m_id;
}


void AMediaItem::setState(AMediaItem::State state)
{
    const bool changed = state != m_state;
    m_state = state;

    if (changed)
        emit stateChanged();
}
