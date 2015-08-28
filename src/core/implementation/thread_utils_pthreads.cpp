
#include "thread_utils.hpp"

#include <pthread.h>
#include <sys/resource.h>

namespace
{
    void set_thread_name(const pthread_t& thread, const std::string& name)
    {
        pthread_setname_np(thread, name.c_str());
    }
    
    void set_thread_priority(const pthread_t& thread, Priority priority)
    {
        int prio = 0;
        
        switch (priority)
        {
            case Priority::VeryHigh: prio = -20; break;
            case Priority::High:     prio = -10; break;
            case Priority::Normal:   prio =   0; break;
            case Priority::Low:      prio =  10; break;
            case Priority::VeryLow:  prio =  19; break;
            
            case Priority::Idle:     prio =  19; break;
        }
        
        setpriority(PRIO_PROCESS, thread, prio);
    }
}


void set_thread_name(std::thread& thread, const std::string& name)
{
    set_thread_name(thread.native_handle(), name);
}


void set_thread_name(const std::string& name)
{
    set_thread_name(pthread_self(), name);
}


void set_thread_priority(std::thread& thread, Priority priority)
{
    set_thread_priority(thread.native_handle(), priority);
}


void set_thread_priority(Priority priority)
{
    set_thread_priority(pthread_self(), priority);
}
