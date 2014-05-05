
#include "thread_multiplexer.hpp"

#include <assert.h>


ThreadMultiplexer::ThreadMultiplexer(): QObject(), IThreadMultiplexer< PhotoInfoUpdater* >(), m_getter(nullptr)
{
    connect(this, SIGNAL(propagate(PhotoInfoUpdater*)), this, SLOT(propagated(PhotoInfoUpdater*)), Qt::QueuedConnection);
}


ThreadMultiplexer::~ThreadMultiplexer()
{

}


void ThreadMultiplexer::setGetter(IThreadMultiplexer< PhotoInfoUpdater* >::IGetter* getter)
{
    m_getter = getter;
}


void ThreadMultiplexer::send(PhotoInfoUpdater* const& photoInfo)
{
    if (m_getter != nullptr)
        emit propagate(photoInfo);
}


void ThreadMultiplexer::propagated(PhotoInfoUpdater* photoInfo)
{
    if (m_getter != nullptr)
        m_getter->signalEmited(photoInfo);
}
