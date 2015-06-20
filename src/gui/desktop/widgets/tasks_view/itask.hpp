
#ifndef ITASK_HPP
#define ITASK_HPP

#include <QString>

class QProgressBar;

struct ITask
{
    virtual ~ITask() {}

    virtual QString       getName() = 0;
    virtual QProgressBar* getProgressBar() = 0;
};

#endif
