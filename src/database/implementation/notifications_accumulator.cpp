
#include "notifications_accumulator.hpp"


void NotificationsAccumulator::photosAdded(const std::vector<Photo::Id>& photos)
{
    m_photosAdded = photos;
}


void NotificationsAccumulator::fireChanges()
{
    emit photosAddedSignal(m_photosAdded);
}


void NotificationsAccumulator::ignoreChanges()
{
    m_photosAdded.clear();
}

