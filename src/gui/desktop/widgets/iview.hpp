
#ifndef IVIEW_HPP
#define IVIEW_HPP

#include <QString>

class QAbstractItemView;
class QItemSelectionModel;

struct IConfiguration;
class DBDataModel;

struct IView
{
    virtual ~IView() {}

    virtual QItemSelectionModel* getSelectionModel() = 0;
    virtual DBDataModel* getModel() = 0;
    virtual QString getName() = 0;

    virtual void set(IConfiguration *) = 0;
};

#endif
