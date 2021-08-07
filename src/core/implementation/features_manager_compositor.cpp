
#include "features_manager_compositor.hpp"


void FeaturesManagerCompositor::add(IFeaturesManager* manager)
{
    m_managers.push_back(manager);
}


bool FeaturesManagerCompositor::has(const QString& feature) const
{
    return std::ranges::any_of(m_managers, [feature](IFeaturesManager* manager)
    {
        return manager->has(feature);
    });
}
