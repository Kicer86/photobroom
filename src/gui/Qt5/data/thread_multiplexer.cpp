
#include "thread_multiplexer.hpp"

#include <assert.h>


ThreadMultiplexer::ThreadMultiplexer(): QObject(), IThreadMultiplexer< PhotoInfoGenerator* >(), m_getter(nullptr)
{
    connect(this, SIGNAL(propagate(PhotoInfoGenerator*)), this, SLOT(propagated(PhotoInfoGenerator*)), Qt::QueuedConnection);
}


ThreadMultiplexer::~ThreadMultiplexer()
{

}


void ThreadMultiplexer::setGetter(IThreadMultiplexer< PhotoInfoGenerator* >::IGetter* getter)
{
    m_getter = getter;
}


void ThreadMultiplexer::send(PhotoInfoGenerator* const& photoInfo)
{
    if (m_getter != nullptr)
        emit propagate(photoInfo);
}


void ThreadMultiplexer::propagated(PhotoInfoGenerator* photoInfo)
{
    if (m_getter != nullptr)
        m_getter->signalEmited(photoInfo);
}
