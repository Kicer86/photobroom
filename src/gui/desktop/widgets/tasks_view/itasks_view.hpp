
#ifndef ITASK_VIEW_HPP
#define ITASK_VIEW_HPP

#include <QString>

class QProgressBar;

struct ITask;


struct ITasksView
{
    virtual ~ITasksView() {}

    virtual void add(ITask *) = 0;
    virtual void finished(ITask *) = 0;
};

#endif
