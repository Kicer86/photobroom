
#include <KCrash/KCrash>

#include "crash_catcher.hpp"


bool CrashCatcher::internal_init()
{
    KCrash::initialize();

    return true;
}
