
#ifndef STOPWATCH_HPP
#define STOPWATCH_HPP

#include <chrono>
#include <utility>

#include "core_export.h"


class CORE_EXPORT Stopwatch final
{
    public:
        using Measure = std::pair<std::chrono::milliseconds, std::chrono::milliseconds>;

        Stopwatch();
        ~Stopwatch();

        void start();
        Measure read();

    private:
        struct Impl;
        std::unique_ptr<Impl> m_impl;
};

#endif
