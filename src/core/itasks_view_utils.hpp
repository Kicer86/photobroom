
#ifndef ITASKS_VIEW_UTILS_HPP_INCLUDED
#define ITASKS_VIEW_UTILS_HPP_INCLUDED


#include "itasks_view.hpp"
#include "core_export.h"

namespace TasksViewUtils
{
    CORE_EXPORT void addFutureTask(ITasksView& view, QFuture<void>&& future);
}

#endif
