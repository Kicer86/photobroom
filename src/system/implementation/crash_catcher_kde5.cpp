
#include <KCrash/KCrash>

#include "crash_catcher.hpp"


void CrashCatcher::internal_init()
{
    KCrash::initialize();
}
