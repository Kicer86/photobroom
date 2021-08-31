
#include <cassert>
#include <boost/chrono.hpp>

#include "stopwatch.hpp"


struct Stopwatch::Impl
{
    std::chrono::time_point<std::chrono::steady_clock> m_lastWall;
    boost::chrono::thread_clock::time_point m_lastCpu;
};


Stopwatch::Stopwatch():
    m_impl(std::make_unique<Impl>())
{

}


Stopwatch::~Stopwatch()
{
}


void Stopwatch::start()
{
    assert(m_impl->m_lastCpu == boost::chrono::thread_clock::time_point());
    assert(m_impl->m_lastWall == std::chrono::time_point<std::chrono::steady_clock>());

    m_impl->m_lastWall = std::chrono::steady_clock::now();
    m_impl->m_lastCpu = boost::chrono::thread_clock::now();
}


Stopwatch::Measure Stopwatch::read()
{
    const auto nowWall = std::chrono::steady_clock::now();
    const auto nowCpu = boost::chrono::thread_clock::now();

    const auto diffWall = nowWall - m_impl->m_lastWall;
    const auto diffWall_ms = std::chrono::duration_cast<std::chrono::milliseconds>(diffWall);

    const auto diffCpu = nowCpu - m_impl->m_lastCpu;
    const auto diffCpu_ms = boost::chrono::duration_cast<boost::chrono::milliseconds>(diffCpu);
    const auto stdDiffCpu_ms = std::chrono::milliseconds(diffCpu_ms.count());

    return Measure(diffWall_ms, stdDiffCpu_ms);
}
