
#include "catcher.hpp"

#include <cstdio>

#include <windows.h>
#include <DbgHelp.h>

#include "crash_dialog_launcher_win32.hpp"

namespace
{
    void bt(EXCEPTION_POINTERS* ExInfo)
    {
        Catcher::launchDialog(ExInfo->ContextRecord);
    }


    LONG WINAPI sig_handler(EXCEPTION_POINTERS* ExceptionInfo)
    {
        switch (ExceptionInfo->ExceptionRecord->ExceptionCode)
        {
            case EXCEPTION_ACCESS_VIOLATION:
            case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
            case EXCEPTION_DATATYPE_MISALIGNMENT:
            case EXCEPTION_FLT_DENORMAL_OPERAND:
            case EXCEPTION_FLT_DIVIDE_BY_ZERO:
            case EXCEPTION_FLT_INEXACT_RESULT:
            case EXCEPTION_FLT_INVALID_OPERATION:
            case EXCEPTION_FLT_OVERFLOW:
            case EXCEPTION_FLT_STACK_CHECK:
            case EXCEPTION_FLT_UNDERFLOW:
            case EXCEPTION_ILLEGAL_INSTRUCTION:

                //case EXCEPTION_IN_PAGE_ERROR:
            case EXCEPTION_INT_DIVIDE_BY_ZERO:
            case EXCEPTION_INT_OVERFLOW:

                //case EXCEPTION_INVALID_DISPOSITION:
            case EXCEPTION_NONCONTINUABLE_EXCEPTION:
            case EXCEPTION_PRIV_INSTRUCTION:

                //case EXCEPTION_SINGLE_STEP:
            case EXCEPTION_STACK_OVERFLOW:
                bt(ExceptionInfo);
                exit(1);
                break;

            default:
                fputs("Error: Unrecognized Exception\n", stderr);
                break;
        }

        return EXCEPTION_EXECUTE_HANDLER;
    }
}


namespace Catcher
{
    void initialize()
    {
        SetUnhandledExceptionFilter(sig_handler);

        int* a = nullptr;
        volatile int b = *a;
    }
}
