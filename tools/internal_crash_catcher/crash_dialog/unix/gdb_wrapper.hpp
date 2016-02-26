
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
        virtual std::vector<QString> getBackTrace() override;

    private:
        const QString m_gdb_path;
        QProcess m_gdb;
        QTemporaryFile m_tmpFile;
        std::vector<QString> m_backtrace;

        void gdbError(QProcess::ProcessError);
        void gdbFinished(int, QProcess::ExitStatus);
};

#endif // GDBWRAPPER_HPP
