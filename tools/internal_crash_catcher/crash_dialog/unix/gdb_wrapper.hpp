
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

        virtual bool attach(qint64 pid, qint64 tid) override;
        virtual bool ready() const override;
        virtual std::vector<std::string> getBackTrace() override;

    private:
        const QString m_gdb_path;
        QProcess m_gdb;
        QTemporaryFile m_tmpFile;

        void gdbReady();
        void gdbError(QProcess::ProcessError);
        void gdbReadyRead();
};

#endif // GDBWRAPPER_HPP
