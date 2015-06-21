
#ifndef ITASK_HPP
#define ITASK_HPP

#include <QString>

class QProgressBar;

struct ITask
{
    virtual ~ITask() {}

    virtual const QString& getName() = 0;
    virtual QProgressBar*  getProgressBar() = 0;

    virtual void finished() = 0;
};

#endif
