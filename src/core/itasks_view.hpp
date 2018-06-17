
#ifndef ITASK_VIEW_HPP
#define ITASK_VIEW_HPP

#include <QString>

class QProgressBar;

struct IViewTask;

struct ITasksView
{
    virtual ~ITasksView() = default;

    virtual IViewTask* add(const QString& name) = 0;
};

#endif
