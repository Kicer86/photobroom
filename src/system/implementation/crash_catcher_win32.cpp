
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

    void   LogStackFrames(void* FaultAddress, char *eNextBP)
    {
        char *p, *pBP;
        unsigned i, x, BpPassed;
        static int  CurrentlyInTheStackDump = 0;

        BpPassed = (eNextBP != nullptr);
        if (eNextBP != nullptr)
        {
            asm
            (
                "movl %%ebp, %0\n\t"
                :: "q"(eNextBP)
            );
        }
        else
            printf("\n  Fault Occurred At $ADDRESS:%08LX\n", (int)FaultAddress);

        // prevent infinite loops
        for(i = 0; eNextBP && i < 100; i++)
        {
            pBP = eNextBP;           // keep current BasePointer
            eNextBP = *(char **)pBP; // dereference next BP
            p = pBP + 8;
            // Write 20 Bytes of potential arguments
            printf("         with ");

            for(x = 0; p < eNextBP && x < 20; p++, x++)
                printf("%02X ", *(unsigned char *)p);

            printf("\n\n");

            if(i == 1 && ! BpPassed)
               printf("*************************************\n"
                                 "         Fault Occurred Here:\n");
            // Write the backjump address
            printf("*** %2d called from $ADDRESS:%08LX\n", i, *(char **)(pBP + 4));

            if(*(char **)(pBP + 4) == NULL)
                break;
        }

    }

    void bt(EXCEPTION_POINTERS* ExInfo)
    {
        MessageBoxA(0, "?", "$3432", MB_OK);

        std::stringstream crashReport;
        crashReport << std::endl;

        unsigned int   i;
        void         * stack[ 100 ];
        unsigned short frames;
        SYMBOL_INFO  * symbol;
        HANDLE         process;

        process = GetCurrentProcess();

        SymInitialize( process, NULL, TRUE );

        frames               = CaptureStackBackTrace( 0, 100, stack, NULL );
        symbol               = ( SYMBOL_INFO * )calloc( sizeof( SYMBOL_INFO ) + 256 * sizeof( char ), 1 );
        symbol->MaxNameLen   = 255;
        symbol->SizeOfStruct = sizeof( SYMBOL_INFO );

        for( i = 0; i < frames; i++ )
        {
            SymFromAddr( process, ( DWORD64 )( stack[ i ] ), 0, symbol );

            crashReport << frames - i - 1 << ": " << symbol->Name << " - " << std::hex << symbol->Address << std::endl;
        }

        free( symbol );

        LogStackFrames(ExInfo->ExceptionRecord->ExceptionAddress,
                       (char *)ExInfo->ContextRecord->Ebp);

        CrashCatcher::saveOutput(crashReport.str());
    }


    LONG WINAPI sig_handler(EXCEPTION_POINTERS* ExceptionInfo)
    {
        switch(ExceptionInfo->ExceptionRecord->ExceptionCode)
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


void CrashCatcher::internal_init()
{
    SetUnhandledExceptionFilter(sig_handler);

    int *a = nullptr;
    volatile int b = *a;
}
