
#include "cross_thread_callback.hpp"

#include <QAbstractEventDispatcher>
#include <QCoreApplication>

namespace FunctorCallConsumer
{
    QObject * forThread(QThread * thread)
    {
        Q_ASSERT(thread);
        QObject * target = thread == qApp->thread()?
            static_cast<QObject*>(qApp):
            QAbstractEventDispatcher::instance(thread);

        Q_ASSERT_X(target, "postMetaCall", "the receiver thread must have an event loop");

        return target;
    }
}
