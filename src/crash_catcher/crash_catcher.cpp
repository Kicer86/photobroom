
#include "crash_catcher.hpp"

#include <iostream>

#include <core/ilogger.hpp>


const char* CrashCatcher::app_name;

bool CrashCatcher::init(const char* name)
{
    app_name = name;

    return internal_init();
}


const char* CrashCatcher::name()
{
    return app_name;
}
