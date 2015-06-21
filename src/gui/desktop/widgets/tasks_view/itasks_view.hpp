
#ifndef ITASK_VIEW_HPP
#define ITASK_VIEW_HPP

#include <QString>

class QProgressBar;

struct ITask;


struct ITasksView
{
    virtual ~ITasksView() {}

    virtual ITask* add(const QString& name) = 0;
};

#endif
