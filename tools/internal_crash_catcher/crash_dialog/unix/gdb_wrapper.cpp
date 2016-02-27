
#include "gdb_wrapper.hpp"

#include <cassert>

#include <QDebug>


GDBWrapper::GDBWrapper(const QString& path):
    m_gdb_path(path),
    m_gdb(),
    m_tmpFile(),
    m_callback(),
    m_exec(),
    m_pid(-1),
    m_tid(-1)
{
    auto errorSignal = static_cast<void (QProcess::*)(QProcess::ProcessError)>(&QProcess::error);
    auto finishedSignal = static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished);

    connect(&m_gdb, errorSignal, this, &GDBWrapper::gdbError);
    connect(&m_gdb, finishedSignal, this, &GDBWrapper::gdbFinished);

    m_gdb.setProcessChannelMode(QProcess::MergedChannels);
}


bool GDBWrapper::attach(qint64 pid, qint64 tid, const QString& exec)
{
    m_tmpFile.open();
    m_tmpFile.write("set width 200\nthread\nthread apply all bt");
    m_tmpFile.write("\n", 1);
    m_tmpFile.flush();

    m_pid = pid;
    m_tid = tid;
    m_exec = exec;
}

void GDBWrapper::requestBacktrace(const std::function<void(const std::vector<QString> &)>& callback)
{
    assert(m_pid > 0);
    assert(m_tid >= 0);

    m_callback = callback;

    QStringList args;
    args << "-p" << QString().setNum(m_pid);
    args << "-nw" << "-n";
    args << "-batch";
    args << "-x" << m_tmpFile.fileName();
    args << m_exec;

    m_gdb.start(m_gdb_path, args);
}



void GDBWrapper::gdbError(QProcess::ProcessError error)
{
    qCritical().noquote() << "Gdb error:" << error;
}


void GDBWrapper::gdbFinished(int, QProcess::ExitStatus)
{
    char buffer[250];
    std::vector<QString> backtrace;

    while(m_gdb.readLine(buffer, 250) > 0)
        backtrace.push_back(buffer);

    m_callback(backtrace);
}
