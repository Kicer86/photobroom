
#ifndef CRASH_CATCHER_HPP
#define CRASH_CATCHER_HPP

class CrashCatcher
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
