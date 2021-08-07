
#ifndef FEATURESMANAGERCOMPOSITOR_HPP
#define FEATURESMANAGERCOMPOSITOR_HPP

#include "ifeatures_manager.hpp"

#include "core_export.h"


class CORE_EXPORT FeaturesManagerCompositor: public IFeaturesManager
{

public:
    virtual ~FeaturesManagerCompositor() = default;

    void add(IFeaturesManager *);
    bool has(const QString &) const override;

private:
    std::vector<IFeaturesManager *> m_managers;
};

#endif
