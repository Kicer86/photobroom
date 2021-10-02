
#include "signal_postponer.hpp"
#include "core_export.h"

// TODO: ugly workaround for a problem with linker causing SignalPostponer class to be removed 
//       from 'core' library although it is being CORE_EXPORTed
CORE_EXPORT void donotuseme();

void donotuseme()
{
    QObject a;
    lazy_connect(&a, &QObject::destroyed, &a, &QObject::dumpObjectTree, std::chrono::seconds(1), std::chrono::seconds(1));
}
