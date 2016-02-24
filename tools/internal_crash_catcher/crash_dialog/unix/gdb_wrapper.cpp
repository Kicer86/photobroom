
#include "gdb_wrapper.hpp"

#include <QDebug>


GDBWrapper::GDBWrapper(const QString& path): m_gdb_path(path), m_gdb(), m_tmpFile()
{
    auto errorSignal = static_cast<void (QProcess::*)(QProcess::ProcessError)>(&QProcess::error);

    connect(&m_gdb, &QProcess::started, this, &GDBWrapper::gdbReady);
    connect(&m_gdb, errorSignal, this, &GDBWrapper::gdbError);
    connect(&m_gdb, &QProcess::readyRead, this, &GDBWrapper::gdbReadyRead);

    m_gdb.setProcessChannelMode(QProcess::MergedChannels);
}


bool GDBWrapper::attach(qint64 pid, qint64 tid, const QString& exec)
{
    m_tmpFile.open();
    m_tmpFile.write("set width 200\nthread\nthread apply all bt");
    m_tmpFile.write("\n", 1);
    m_tmpFile.flush();

    QStringList args;
    args << "-p" << QString().setNum(pid);
    args << "-nw" << "-n";
    args << "-batch";
    args << "-x" << m_tmpFile.fileName();
    args << exec;

    m_gdb.start(m_gdb_path, args);
}


bool GDBWrapper::ready() const
{
    return m_gdb.state() == QProcess::Running;
}


std::vector<std::string> GDBWrapper::getBackTrace()
{
    return std::vector<std::string>();
}


void GDBWrapper::gdbReady()
{

}


void GDBWrapper::gdbError(QProcess::ProcessError error)
{
    qCritical().noquote() << "Gdb error:" << error;
}


void GDBWrapper::gdbReadyRead()
{
    QByteArray output = m_gdb.readAll();

    qDebug().noquote() << output;
}
