

#ifndef IMODEL_COMPOSITOR_DATA_SOURCE_HPP_INCLUDED
#define IMODEL_COMPOSITOR_DATA_SOURCE_HPP_INCLUDED

#include <QObject>

class IModelCompositorDataSource: public QObject
{
    Q_OBJECT

public:
    virtual const QStringList& data() const = 0;

signals:
    void dataChanged() const;
};


#endif // IMODEL_COMPOSITOR_DATA_SOURCE_HPP_INCLUDED
