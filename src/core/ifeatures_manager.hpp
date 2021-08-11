
#ifndef IFEATURES_MANAGER_HPP
#define IFEATURES_MANAGER_HPP

#include <QStringList>
#include <QObject>

#include <core_export.h>


class CORE_EXPORT IFeaturesManager: public QObject
{
    Q_OBJECT

public:
    virtual ~IFeaturesManager() = default;

    virtual bool has(const QString &) const = 0;

signals:
    void featureChanged(const QString& key, bool value) const;
};

#endif
