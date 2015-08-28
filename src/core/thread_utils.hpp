
#ifndef THREAD_UTILS_HPP
#define THREAD_UTILS_HPP

#include <thread>
#include <string>

#include "core_export.h"

enum class Priority
{
    VeryHigh,
    High,
    Normal,
    Low,
    VeryLow,
    Idle
};
    
void CORE_EXPORT set_thread_name(std::thread &, const std::string &);
void CORE_EXPORT set_thread_name(const std::string &);

void CORE_EXPORT set_thread_priority(std::thread &, Priority);
void CORE_EXPORT set_thread_priority(Priority);

#endif // THREAD_UTILS_HPP
