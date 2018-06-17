
#ifndef IVIEW_TASK_HPP
#define IVIEW_TASK_HPP

#include <QString>


struct IProgressBar
{
    virtual ~IProgressBar() = default;

    virtual void setMinimum(int) = 0;
    virtual void setMaximum(int) = 0;
    virtual void setValue(int) = 0;
};


struct IViewTask
{
    virtual ~IViewTask() = default;

    virtual const QString& getName() = 0;
    virtual IProgressBar*  getProgressBar() = 0;

    virtual void finished() = 0;
};

#endif
