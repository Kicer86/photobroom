
#include "features_manager_compositor.hpp"


void FeaturesManagerCompositor::add(IFeaturesManager* manager)
{
    m_managers.push_back(manager);

    connect(manager, &IFeaturesManager::featureChanged, this, &FeaturesManagerCompositor::featureChanged);
}


bool FeaturesManagerCompositor::has(const QString& feature) const
{
    return std::ranges::any_of(m_managers, [feature](IFeaturesManager* manager)
    {
        return manager->has(feature);
    });
}
