
#include "catcher.hpp"

#include <unistd.h>
#include <sys/prctl.h>
#include <sys/wait.h>

#include <cassert>
#include <csignal>
#include <iostream>

#include <QCoreApplication>
#include <QFileInfo>
#include <QStandardPaths>


// http://stackoverflow.com/questions/35976496/sigactions-signal-handler-not-called-in-child-process/35976803
// http://man7.org/linux/man-pages/man7/signal.7.html -> Async-signal-safe functions

namespace
{
    std::string crashDialog;
    std::string app_path;
    std::string app_pid;

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

                sigset_t sigs;
                sigaddset(&sigs, signo);
                sigprocmask(SIG_UNBLOCK, &sigs, nullptr);

                const pid_t fp = fork();

                if (fp == 0)
                    execl(crashDialog.c_str(), crashDialog.c_str(),
                          "-p", app_pid.c_str(),
                          "-t", "0",
                          "-e", app_path.c_str(),
                          nullptr
                         );

                waitpid(fp, nullptr, 0);

                _exit(1);

                break;
            }

            default:
                break;
        }

    }
}


namespace Catcher
{
    bool initialize()
    {
        bool status = false;

        const QString crashDialogExecutable = QStandardPaths::findExecutable("crash_dialog");

        if (crashDialogExecutable.isEmpty())
            std::cerr << "Could not find crash_dialog exec" << std::endl;
        else
        {
            const pid_t pid = getpid();
            const QFileInfo fileInfo(crashDialogExecutable);

            app_path = QCoreApplication::arguments().at(0).toStdString();
            app_pid = std::to_string(pid);
            crashDialog = fileInfo.absoluteFilePath().toStdString();

            status = true;

            struct sigaction act;
            sigemptyset(&act.sa_mask);

            act.sa_handler = sig_handler;
            act.sa_flags = 0;

            status &= sigaction(SIGSEGV, &act, 0) == 0;
            status &= sigaction(SIGILL, &act, 0) == 0;
            status &= sigaction(SIGABRT, &act, 0) == 0;
            status &= sigaction(SIGFPE, &act, 0) == 0;
        }

        return status;
    }
}

