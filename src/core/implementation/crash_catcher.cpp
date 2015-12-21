
#include "crash_catcher.hpp"

#include <iostream>

#include "ilogger.hpp"

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


void CrashCatcher::saveOutput(const std::stringstream& report)
{
    std::cout << report.str();
    logger->error(report.str());
}
