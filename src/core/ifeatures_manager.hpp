
#ifndef IFEATURES_MANAGER_HPP
#define IFEATURES_MANAGER_HPP

#include <QStringList>


struct IFeatureDetector
{
    virtual ~IFeatureDetector() = default;

    virtual QStringList detect() = 0;
};


struct IFeaturesManager
{
    virtual ~IFeaturesManager() = default;

    virtual bool has(const QString &) const = 0;

    virtual void add(IFeatureDetector *) = 0;
    virtual void detect() = 0;
};

#endif
