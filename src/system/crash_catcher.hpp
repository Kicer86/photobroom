
#ifndef CRASH_CATCHER_HPP
#define CRASH_CATCHER_HPP

#include <sstream>

#include "system_export.h"

class SYSTEM_EXPORT CrashCatcher
{
        CrashCatcher();
        ~CrashCatcher();

    public:
        static void init(const char *);
        static const char* name();

    private:
        static const char* app_name;

        static void internal_init();
};

#endif
