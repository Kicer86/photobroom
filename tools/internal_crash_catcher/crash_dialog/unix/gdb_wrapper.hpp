
#ifndef GDBWRAPPER_HPP
#define GDBWRAPPER_HPP

#include <QProcess>
#include <QString>
#include <QTemporaryFile>

#include "../idebugger.hpp"


class GDBWrapper: public QObject, public IDebugger
{
        Q_OBJECT

    public:
        GDBWrapper(const QString &);

        virtual bool attach(qint64 pid, qint64 tid, const QString& exec) override;
        virtual void requestBacktrace(const std::function<void(const std::vector<QString> &)> &) override;

        virtual const QString& exec() const override;

    private:
        const QString m_gdb_path;
        QProcess m_gdb;
        QTemporaryFile m_tmpFile;
        std::function<void(const std::vector<QString> &)> m_callback;
        QString m_exec;
        qint64 m_pid;
        qint64 m_tid;

        void gdbError(QProcess::ProcessError);
        void gdbFinished(int, QProcess::ExitStatus);
};

#endif // GDBWRAPPER_HPP
