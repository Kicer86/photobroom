
#include "notifications_accumulator.hpp"

namespace Database
{

    void NotificationsAccumulator::photosAdded(const std::vector<Photo::Id>& photos)
    {
        m_photosAdded.insert(m_photosAdded.end(), photos.begin(), photos.end());
    }


    void NotificationsAccumulator::photosModified(const std::set<Photo::Id>& photos)
    {
        m_photosModified.insert(photos.begin(), photos.end());
    }

    void NotificationsAccumulator::fireChanges()
    {
        emit photosAddedSignal(m_photosAdded);
        emit photosModifiedSignal(m_photosModified);
    }


    void NotificationsAccumulator::ignoreChanges()
    {
        m_photosAdded.clear();
        m_photosModified.clear();
    }

}
