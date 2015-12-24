
#include "crash_catcher.hpp"

#include <execinfo.h>
#include <unistd.h>

#include <cassert>
#include <csignal>
#include <cstdio>
#include <cstring>

#include <iostream>

#include <libbacktrace/backtrace.h>
#include <libbacktrace/backtrace-supported.h>

namespace
{

    struct bt_ctx
    {
        struct backtrace_state *state;
        int error;
    };


    void error_callback(void *data, const char *msg, int errnum)
    {
        struct bt_ctx *ctx = static_cast<struct bt_ctx *>(data);
        fprintf(stderr, "ERROR: %s (%d)", msg, errnum);
        ctx->error = 1;
    }


    void bt(struct backtrace_state *state)
    {
        char buff[10240];
        memset(buff, 0, 10240);
        FILE* mem_file = fmemopen(buff, 10240, "w");

        backtrace_print(state, 0, mem_file);

        fclose(mem_file);

        const std::string data(buff);
        CrashCatcher::saveOutput(data);
    }


    void sig_handler(int signo)
    {
        switch(signo)
        {
            case SIGSEGV:
            case SIGILL:
            case SIGABRT:
            case SIGFPE:
            {
                struct backtrace_state *state = backtrace_create_state (CrashCatcher::name(), BACKTRACE_SUPPORTS_THREADS, error_callback, NULL);
                bt(state);

                exit(1);
                break;
            }

            default:
                break;
        }
    }
}


void CrashCatcher::internal_init()
{
    bool status = true;

    struct sigaction act;
    sigemptyset(&act.sa_mask);

    act.sa_handler = sig_handler;
    act.sa_flags = 0;

    status &= sigaction(SIGSEGV, &act, 0) == 0;
    status &= sigaction(SIGILL, &act, 0) == 0;
    status &= sigaction(SIGABRT, &act, 0) == 0;
    status &= sigaction(SIGFPE, &act, 0) == 0;

    assert(status);
}
