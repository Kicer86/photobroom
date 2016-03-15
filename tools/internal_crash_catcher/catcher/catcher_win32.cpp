
#include "catcher.hpp"

#include <cstdio>
#include <iostream>

#include <windows.h>
#include <DbgHelp.h>

#include <QCoreApplication>
#include <QDebug>
#include <QProcess>
#include <QStandardPaths>
#include <QStringList>

#include "crash_dialog/win32/kdbgwin/config.h"


namespace
{
    QString crashDialog;

    void bt(EXCEPTION_POINTERS* ExInfo)
    {
        HANDLE hMapFile = CreateFileMapping(
                           INVALID_HANDLE_VALUE,
                           NULL,
                           PAGE_READWRITE,
                           0,
                           sizeof(CONTEXT),
                           TEXT(sharedMemoryName)
                  );

        LPVOID pBuf = MapViewOfFile(
                           hMapFile,
                           FILE_MAP_ALL_ACCESS,
                           0,
                           0,
                           sizeof(CONTEXT)
                      );

        CopyMemory(pBuf, ExInfo->ContextRecord, sizeof(CONTEXT));

        // launch dialog
        const DWORD pid = GetCurrentProcessId();
        const DWORD tid = GetCurrentThreadId();

        QStringList args;

        args << "--pid" << QString().number(pid);
        args << "--tid" << QString().number(tid);
        args << "--exec" << QCoreApplication::arguments().at(0);

        qDebug().noquote() << "Crash catcher: executing:" << crashDialog << args;

        QProcess::execute(crashDialog, args);

        CloseHandle(hMapFile);
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
    bool initialize()
    {
        bool status = false;

        crashDialog = QStandardPaths::findExecutable("crash_dialog", { QCoreApplication::applicationDirPath() } );

        if (crashDialog.isEmpty())
            std::cerr << "Could not find crash_dialog exec" << std::endl;
        else
        {
            SetUnhandledExceptionFilter(sig_handler);
            status = true;
        }

        return status;
    }
}
