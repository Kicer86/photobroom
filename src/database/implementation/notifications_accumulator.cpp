
#include "notifications_accumulator.hpp"

namespace Database
{

    void NotificationsAccumulator::photosAdded(const std::vector<Photo::Id>& photos)
    {
        m_photosAdded.insert(m_photosAdded.end(), photos.begin(), photos.end());
    }


    void NotificationsAccumulator::fireChanges()
    {
        emit photosAddedSignal(m_photosAdded);
    }


    void NotificationsAccumulator::ignoreChanges()
    {
        m_photosAdded.clear();
    }

}
