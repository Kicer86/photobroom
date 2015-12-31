
#include "crash_catcher.hpp"

#include <cassert>
#include <csignal>
#include <sstream>
#include <vector>

#include <windows.h>
#include <DbgHelp.h>

// Links:
// http://spin.atomicobject.com/2013/01/13/exceptions-stack-traces-c/
// http://www.drdobbs.com/tools/postmortem-debugging/185300443
// https://oroboro.com/stack-trace-on-crash/
// https://stackwalker.codeplex.com/

// http://stackoverflow.com/questions/22481126/why-isnt-symgetsymfromaddr64-working-it-returns-error-code-126
// http://stackoverflow.com/questions/13437158/obtaining-frame-pointer-in-c

namespace
{

    struct FunctionInfo
    {
        void* ptr;
        std::string name;
    };


    std::vector<FunctionInfo> findFunctionNames(const std::vector<void *>& ptrs)
    {
        std::vector<FunctionInfo> result;
        HANDLE process = GetCurrentProcess();

        SYMBOL_INFO* symbol = ( SYMBOL_INFO * )calloc( sizeof( SYMBOL_INFO ) + 256 * sizeof( char ), 1 );
        symbol->MaxNameLen   = 255;
        symbol->SizeOfStruct = sizeof( SYMBOL_INFO );

        for( std::size_t i = 0; i < ptrs.size(); i++ )
        {
            SymFromAddr( process, DWORD64(ptrs[i]), 0, symbol );

            FunctionInfo info;
            info.ptr = ptrs[i];
            info.name = symbol->Name;

            result.push_back(info);
        }

        free(symbol);

        return result;
    }

    struct stack_frame {
            struct stack_frame *prev;
            void *return_addr;
    } __attribute__((packed));
    typedef struct stack_frame stack_frame;

    std::vector<void *> backtrace_from_fp(void* rfp)
    {
        std::vector<void *> result;

        for(stack_frame *fp = static_cast<stack_frame *>(rfp); fp != nullptr; fp = fp->prev)
            result.push_back(fp->return_addr);

        return result;
    }

    std::vector<void *> LogStackFrames(PCONTEXT context)
    {
        std::vector<void *> result;

        HANDLE process = GetCurrentProcess();
        HANDLE thread  = GetCurrentThread();
        STACKFRAME64 stackFrame;
        ZeroMemory(&stackFrame, sizeof(stackFrame));

        stackFrame.AddrPC.Offset    = context->Eip;
        stackFrame.AddrPC.Mode      = AddrModeFlat;
        stackFrame.AddrStack.Offset = context->Esp;
        stackFrame.AddrStack.Mode   = AddrModeFlat;
        stackFrame.AddrFrame.Offset = context->Ebp;
        stackFrame.AddrFrame.Mode   = AddrModeFlat;

        for(;;)
        {
            BOOL status = StackWalk64
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

            result.push_back( reinterpret_cast<void *>(stackFrame.AddrPC.Offset) );

            if( !status )
                break;
        }

        return result;
    }

    void bt(EXCEPTION_POINTERS* ExInfo)
    {
        MessageBoxA(0, "?", "$3432", MB_OK);

        std::stringstream crashReport;
        crashReport << std::endl;

        void         * stack[ 100 ];
        HANDLE         process;

        process = GetCurrentProcess();

        SymInitialize(process, NULL, TRUE);

        int frames = CaptureStackBackTrace( 0, 100, stack, NULL );

        std::vector<void *> stack1(&stack[0], &stack[frames]);
        std::vector<FunctionInfo> info1 = findFunctionNames(stack1);

        std::vector<void *> stack2 = backtrace_from_fp(reinterpret_cast<void *>(ExInfo->ContextRecord->Ebp));
        std::vector<FunctionInfo> info2 = findFunctionNames(stack2);

        std::vector<void *> stack3 = LogStackFrames(ExInfo->ContextRecord);
        std::vector<FunctionInfo> info3 = findFunctionNames(stack3);


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
