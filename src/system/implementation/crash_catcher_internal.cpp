
#include "crash_catcher.hpp"

#include "internal_crash_catcher/catcher/catcher.hpp"

bool CrashCatcher::internal_init()
{
    return Catcher::initialize();
}
