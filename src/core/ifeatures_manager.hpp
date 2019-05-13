
#ifndef IFEATURES_MANAGER_HPP
#define IFEATURES_MANAGER_HPP


struct IFeaturesManager
{
    virtual ~IFeaturesManager() = default;

    virtual bool has(const QString &) const = 0;
};

#endif
