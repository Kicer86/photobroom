
#ifndef CRASH_CATCHER_HPP
#define CRASH_CATCHER_HPP

#include <sstream>

#include "crash_catcher_export.h"

class CRASH_CATCHER_EXPORT CrashCatcher
{
        CrashCatcher();
        ~CrashCatcher();

    public:
        static bool init(const char *);
        static const char* name();

    private:
        static const char* app_name;

        static bool internal_init();
};

#endif
