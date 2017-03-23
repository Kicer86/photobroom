
#ifndef STOPWATCH_HPP
#define STOPWATCH_HPP

#include <chrono>

#include "core_export.h"

class CORE_EXPORT Stopwatch final
{
    public:
        Stopwatch();
        ~Stopwatch();

        void start();
        int read(bool reset = false);
        int stop();

    private:
        std::chrono::time_point<std::chrono::steady_clock> m_last;
};

#endif
