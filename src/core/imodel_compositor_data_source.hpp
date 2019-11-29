

#ifndef IMODEL_COMPOSITOR_DATA_SOURCE_HPP_INCLUDED
#define IMODEL_COMPOSITOR_DATA_SOURCE_HPP_INCLUDED


struct IModelCompositorDataSource
{
    virtual const QStringList& data() const = 0;
};


#endif // IMODEL_COMPOSITOR_DATA_SOURCE_HPP_INCLUDED
