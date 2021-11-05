
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


    void NotificationsAccumulator::photosRemoved(const std::vector<Photo::Id>& photos)
    {
        m_photosRemoved.insert(m_photosRemoved.end(), photos.begin(), photos.end());
    }


    void NotificationsAccumulator::fireChanges()
    {
        raiseDeletion();

        if (m_photosAdded.empty() == false)
            emit photosAddedSignal(m_photosAdded);

        if (m_photosModified.empty() == false)
            emit photosModifiedSignal(m_photosModified);

        if (m_photosRemoved.empty() == false)
            emit photosRemovedSignal(m_photosRemoved);

        clearNotifications();
    }


    void NotificationsAccumulator::ignoreChanges()
    {
        clearNotifications();
    }


    void NotificationsAccumulator::clearNotifications()
    {
        m_photosAdded.clear();
        m_photosModified.clear();
        m_photosRemoved.clear();
    }


    void NotificationsAccumulator::raiseDeletion()
    {
        for(const Photo::Id& id: m_photosRemoved)
        {
            std::erase(m_photosAdded, id);
            m_photosModified.erase(id);
        }
    }

}
