
#ifndef IFEATURES_MANAGER_HPP
#define IFEATURES_MANAGER_HPP

#include <QStringList>

struct IFeaturesManager
{
    virtual ~IFeaturesManager() = default;

    virtual bool has(const QString &) const = 0;

    virtual void add(const QString &) = 0;
};

#endif
