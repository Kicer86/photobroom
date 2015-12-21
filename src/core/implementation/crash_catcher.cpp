
#include "crash_catcher.hpp"

const char* CrashCatcher::app_name;
ILogger* CrashCatcher::logger;

void CrashCatcher::init(const char* name, ILogger* l)
{
    app_name = name;
    logger = l;

    internal_init();
}


const char* CrashCatcher::name()
{
    return app_name;
}


ILogger* CrashCatcher::getLogger()
{
    return logger;
}
