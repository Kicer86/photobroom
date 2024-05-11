
#include "features_observer.hpp"

#include "gui/features.hpp"


FeaturesObserver::FeaturesObserver(IFeaturesManager& features, INotifications& notifications)
    : m_notifications(notifications)
{
    connect(&features, &IFeaturesManager::featureChanged, this, &FeaturesObserver::featureChanged);
}


void FeaturesObserver::featureChanged(const QString& /*feature*/, bool /*value*/)
{

}
