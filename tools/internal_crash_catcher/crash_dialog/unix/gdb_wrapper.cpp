
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
    connect(&m_gdb, &QProcess::errorOccurred, this, &GDBWrapper::gdbError);
    connect(&m_gdb, &QProcess::finished, this, &GDBWrapper::gdbFinished);

    m_gdb.setProcessChannelMode(QProcess::MergedChannels);
}


bool GDBWrapper::init(qint64 pid, qint64 tid, const QString& exec)
{
    const bool status = m_tmpFile.open();

    if (status)
    {
      m_tmpFile.write("set width 200\nthread\nthread apply all bt");
      m_tmpFile.write("\n", 1);
      m_tmpFile.flush();

      m_pid = pid;
      m_tid = tid;
      m_exec = exec;
    }

    return status;
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


const QString& GDBWrapper::exec() const
{
    return m_exec;
}


void GDBWrapper::gdbError(QProcess::ProcessError error)
{
    qCritical().noquote() << "Gdb error:" << error;
}


void GDBWrapper::gdbFinished(int, QProcess::ExitStatus)
{
    std::vector<QString> backtrace;

    for(;;)
    {
        const QByteArray line = m_gdb.readLine();

        if (line.isEmpty())
            break;

        const QString lineStr = line.constData();
        backtrace.push_back( lineStr.trimmed() );
    }

    m_callback(backtrace);
}
