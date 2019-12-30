

#ifndef IMODEL_COMPOSITOR_DATA_SOURCE_HPP_INCLUDED
#define IMODEL_COMPOSITOR_DATA_SOURCE_HPP_INCLUDED

#include <QObject>

#include "core_export.h"


class CORE_EXPORT IModelCompositorDataSource: public QObject
{
    Q_OBJECT

public:
    virtual const QStringList& data() const = 0;

signals:
    void dataChanged() const;
};


#endif // IMODEL_COMPOSITOR_DATA_SOURCE_HPP_INCLUDED
