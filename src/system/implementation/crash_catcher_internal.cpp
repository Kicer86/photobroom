
#include "crash_catcher.hpp"

#include "internal_crash_catcher/catcher/catcher.hpp"

void CrashCatcher::internal_init()
{
    Catcher::initialize();
}
