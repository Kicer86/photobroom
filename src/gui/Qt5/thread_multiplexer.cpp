
#include "thread_multiplexer.hpp"

#include <assert.h>


ThreadMultiplexer::ThreadMultiplexer(): QObject(), IThreadMultiplexer< PhotoInfo* >(), m_getter(nullptr)
{

}


ThreadMultiplexer::~ThreadMultiplexer()
{

}


void ThreadMultiplexer::setGetter(IThreadMultiplexer< PhotoInfo* >::IGetter* getter)
{
    m_getter = getter;
}


void ThreadMultiplexer::send(PhotoInfo* const& photoInfo)
{
    assert(m_getter != nullptr);
    emit propagate(photoInfo);
}


void ThreadMultiplexer::propagated(PhotoInfo* photoInfo)
{
    assert(m_getter != nullptr);
    m_getter->getSignal(photoInfo);
}
