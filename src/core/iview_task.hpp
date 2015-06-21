
#ifndef IVIEW_TASK_HPP
#define IVIEW_TASK_HPP

#include <QString>

class QProgressBar;

struct IViewTask
{
    virtual ~IViewTask() {}

    virtual const QString& getName() = 0;
    virtual QProgressBar*  getProgressBar() = 0;

    virtual void finished() = 0;
};

#endif
