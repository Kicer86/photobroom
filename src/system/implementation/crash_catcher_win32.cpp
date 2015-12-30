
#include "crash_catcher.hpp"

#include <cassert>
#include <csignal>
#include <sstream>

#include <windows.h>
#include <DbgHelp.h>

// Links:
// http://spin.atomicobject.com/2013/01/13/exceptions-stack-traces-c/
// http://www.drdobbs.com/tools/postmortem-debugging/185300443
// https://oroboro.com/stack-trace-on-crash/
// https://stackwalker.codeplex.com/


namespace
{

    struct stack_frame {
            struct stack_frame *prev;
            void *return_addr;
    } __attribute__((packed));
    typedef struct stack_frame stack_frame;

    void backtrace_from_fp(void **buf, int size, void* rfp)
    {
            int i;
            stack_frame *fp = static_cast<stack_frame *>(rfp);

            for(i = 0; i < size && fp != NULL; fp = fp->prev, i++)
                    buf[i] = fp->return_addr;
    }

    void   LogStackFrames(PCONTEXT context)
    {
        HANDLE process = GetCurrentProcess();
        HANDLE thread  = GetCurrentThread();
        DWORD64 displacement = 0;
        char name[256];
        STACKFRAME64 stackFrame;
        ZeroMemory(&stackFrame, sizeof(stackFrame));

        stackFrame.AddrPC.Offset    = context->Eip;
        stackFrame.AddrPC.Mode      = AddrModeFlat;
        stackFrame.AddrStack.Offset = context->Esp;
        stackFrame.AddrStack.Mode   = AddrModeFlat;
        stackFrame.AddrFrame.Offset = context->Ebp;
        stackFrame.AddrFrame.Mode   = AddrModeFlat;

        for( int frame = 0; ; frame++ )
        {
            BOOL result = StackWalk64
            (
                IMAGE_FILE_MACHINE_I386,
                process,
                thread,
                &stackFrame,
                context,
                nullptr,
                SymFunctionTableAccess64,
                SymGetModuleBase64,
                nullptr
            );

            SYMBOL_INFO* symbol = ( SYMBOL_INFO * )calloc( sizeof( SYMBOL_INFO ) + 256 * sizeof( char ), 1 );
            symbol->MaxNameLen   = 255;
            symbol->SizeOfStruct = sizeof( SYMBOL_INFO );

            BOOL s1 = SymFromAddr( process, ( ULONG64 )stackFrame.AddrPC.Offset, &displacement, symbol );
            DWORD err = GetLastError();
            DWORD s2 = UnDecorateSymbolName( symbol->Name, ( PSTR )name, 256, UNDNAME_COMPLETE );

            printf
            (
                "Frame %lu:\n"
                "    Symbol name:    %s\n"
                "    PC address:     0x%08LX\n"
                "    Stack address:  0x%08LX\n"
                "    Frame address:  0x%08LX\n"
                "\n",
                frame,
                symbol->Name,
                ( ULONG64 )stackFrame.AddrPC.Offset,
                ( ULONG64 )stackFrame.AddrStack.Offset,
                ( ULONG64 )stackFrame.AddrFrame.Offset
            );

            free(symbol);

            if( !result )
            {
                break;
            }
        }

    }

    void bt(EXCEPTION_POINTERS* ExInfo)
    {
        MessageBoxA(0, "?", "$3432", MB_OK);

        std::stringstream crashReport;
        crashReport << std::endl;

        unsigned int   i;
        void*          stack[ 100 ];
        unsigned short frames;
        SYMBOL_INFO*   symbol;
        HANDLE         process;

        process = GetCurrentProcess();

        SymInitialize(process, NULL, TRUE);

        frames               = CaptureStackBackTrace(0, 100, stack, NULL);
        symbol               = (SYMBOL_INFO*)calloc(sizeof(SYMBOL_INFO) + 256 * sizeof(char), 1);
        symbol->MaxNameLen   = 255;
        symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

        for (i = 0; i < frames; i++)
        {
            SymFromAddr(process, (DWORD64)(stack[ i ]), 0, symbol);

            crashReport << frames - i - 1 << ": " << symbol->Name << " - " << std::hex << symbol->Address << std::endl;
        }

        free(symbol);

        backtrace_from_fp(stack, 100, reinterpret_cast<void *>(ExInfo->ContextRecord->Ebp));

        LogStackFrames(ExInfo->ContextRecord);

        CrashCatcher::saveOutput(crashReport.str());
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

            default
                    :
                fputs("Error: Unrecognized Exception\n", stderr);
                break;
        }

        return EXCEPTION_EXECUTE_HANDLER;
    }
}


void CrashCatcher::internal_init()
{
    SetUnhandledExceptionFilter(sig_handler);

    int* a = nullptr;
    volatile int b = *a;
}
