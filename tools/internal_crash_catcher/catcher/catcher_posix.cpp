
#include "catcher.hpp"

#include <unistd.h>
#include <sys/prctl.h>

#include <cassert>
#include <csignal>
#include <iostream>

#include <QCoreApplication>
#include <QDebug>
#include <QFileInfo>
#include <QProcess>
#include <QStandardPaths>


namespace
{
    QString crashDialog;

    void sig_handler(int signo)
    {
        switch(signo)
        {
            case SIGSEGV:
            case SIGILL:
            case SIGABRT:
            case SIGFPE:
            {
                const pid_t pid = getpid();

                prctl(PR_SET_PTRACER, pid, 0, 0, 0);

                QStringList args;

                args << "-p" << QString().number(pid);
                args << "-t" << "0";
                args << "-e" << QCoreApplication::arguments().at(0);

                qDebug().noquote() << "Crash catcher: executing:" << crashDialog << args;

                QProcess::execute(crashDialog, args);

                exit(1);
                break;
            }

            default:
                break;
        }
    }
}


namespace Catcher
{
    void initialize()
    {
        crashDialog = QStandardPaths::findExecutable("crash_dialog");

        if (crashDialog.isEmpty())
            std::cerr << "Could not find crash_dialog exec" << std::endl;
        else
        {
            QFileInfo fileInfo(crashDialog);
            crashDialog = fileInfo.absoluteFilePath();

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
    }
}

