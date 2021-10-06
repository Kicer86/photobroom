
#include <QFutureWatcher>

#include "iview_task.hpp"
#include "itasks_view_utils.hpp"


namespace TasksViewUtils
{
    void addFutureTask(ITasksView& view, const QFuture<void>& future)
    {
        auto* task = view.add(future.progressText());
        auto* progressBar = task->getProgressBar();

        progressBar->setMinimum(future.progressMinimum());
        progressBar->setMaximum(future.progressMaximum());
        progressBar->setValue(future.progressValue());

        QFutureWatcher<void>* watcher = new QFutureWatcher<void>;
        watcher->setFuture(future);

        QObject::connect(watcher, &QFutureWatcher<void>::progressValueChanged, [progressBar](int v) { progressBar->setValue(v); });
        QObject::connect(watcher, &QFutureWatcher<void>::progressRangeChanged, [progressBar](int min, int max) { progressBar->setMinimum(min); progressBar->setMaximum(max); });
        QObject::connect(watcher, &QFutureWatcher<void>::finished, watcher, &QFutureWatcher<void>::deleteLater);
        QObject::connect(watcher, &QFutureWatcher<void>::finished, [task]() { task->finished(); });
    }
}
