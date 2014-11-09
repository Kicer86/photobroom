
#ifndef IVIEW_HPP
#define IVIEW_HPP

#include <QString>

class QAbstractItemView;

struct IConfiguration;
class DBDataModel;

struct IView
{
    virtual ~IView() {}

    virtual QAbstractItemView* getView() = 0;
    virtual QString getName() = 0;

    virtual void set(IConfiguration *) = 0;
};

#endif
