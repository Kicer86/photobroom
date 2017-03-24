
#include <cassert>

#include "stopwatch.hpp"

Stopwatch::Stopwatch():
    m_last()
{

}


Stopwatch::~Stopwatch()
{
}


void Stopwatch::start()
{
    assert(m_last == std::chrono::time_point<std::chrono::steady_clock>());
    m_last = std::chrono::steady_clock::now();
}


int Stopwatch::read(bool reset)
{
    const auto now = std::chrono::steady_clock::now();

    if (reset)
        m_last = now;

    const auto diff = now - m_last;
    const auto diff_ms = std::chrono::duration_cast<std::chrono::milliseconds>(diff).count();

    return diff_ms;
}


int Stopwatch::stop()
{
    const int result = read();
    m_last = std::chrono::time_point<std::chrono::steady_clock>();

    return result;
}
