
#include "crash_catcher.hpp"

#include <iostream>

#include <core/ilogger.hpp>

const char* CrashCatcher::app_name;

void CrashCatcher::init(const char* name)
{
    app_name = name;

    internal_init();
}


const char* CrashCatcher::name()
{
    return app_name;
}
