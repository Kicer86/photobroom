
#ifndef ITASK_VIEW_HPP
#define ITASK_VIEW_HPP

#include <QString>

#include "core_export.h"

class QProgressBar;

struct IViewTask;


struct CORE_EXPORT ITasksView
{
    virtual ~ITasksView() = default;

    virtual IViewTask* add(const QString& name) = 0;
};

#endif
