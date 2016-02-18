
#ifndef IVIEW_TASK_HPP
#define IVIEW_TASK_HPP

#include <QString>

#include "core_export.h"

struct CORE_EXPORT IProgressBar
{
    virtual ~IProgressBar();

    virtual void setMinimum(int) = 0;
    virtual void setMaximum(int) = 0;
    virtual void setValue(int) = 0;
};


struct CORE_EXPORT IViewTask
{
    virtual ~IViewTask();

    virtual const QString& getName() = 0;
    virtual IProgressBar*  getProgressBar() = 0;

    virtual void finished() = 0;
};

#endif
