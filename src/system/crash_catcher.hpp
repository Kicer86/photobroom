
#ifndef CRASH_CATCHER_HPP
#define CRASH_CATCHER_HPP

#include <sstream>

struct ILogger;

class CrashCatcher
{
        CrashCatcher();
        ~CrashCatcher();

    public:
        static void init(const char *, ILogger *);
        static const char* name();

        static void saveOutput(const std::string &);

    private:
        static const char* app_name;
        static ILogger* logger;

        static void internal_init();
};

#endif
