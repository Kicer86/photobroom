
#include <iostream>

#include <QApplication>
#include <QDebug>

#ifdef DEVELOPER_BUILD
#include <QProcess>
#include "crash_dialog_config.hpp"
#endif

#include "ui/crash_dialog.hpp"
#include "command_line_parser.hpp"
#include "debugger_factory.hpp"


#ifdef DEVELOPER_BUILD
struct DummyExecInfoProvider
{
    DummyExecInfoProvider(const QProcess& proc):
        m_pid(proc.processId()),
        m_tid(0),
        m_exec(proc.program())
    {
    }

    bool error() const { return false; }
    QString errorMsg() const { return QString(); }

    qint64 pid() const { return m_pid; }
    qint64 tid() const { return m_tid; }
    const QString& exec() const { return m_exec; }

    qint64 m_pid, m_tid;
    QString m_exec;
};
#endif


int main(int argc, char** argv)
{
    int result = 1;

    QApplication app(argc, argv);
    QApplication::setApplicationName("CrashDialog");
    QApplication::setApplicationVersion("1.0");

#ifdef DEVELOPER_BUILD
    QProcess dummy_exec;
    dummy_exec.start(test_app_path);
    dummy_exec.pid();
    DummyExecInfoProvider parser(dummy_exec);
#else
    CommandLineParser parser(app);
#endif

    if (parser.error())
        std::cerr << parser.errorMsg().toStdString() << std::endl;
    else
    {
        DebuggerFactory dbgFactory;
        std::unique_ptr<IDebugger> debugger = dbgFactory.get();

        qDebug().noquote() << "Attaching to: " << parser.exec() << ", pid:" << parser.pid();
        debugger->init(parser.pid(), parser.tid(), parser.exec());

        CrashDialog dialog(debugger.get());
        dialog.show();

        result = app.exec();
    }

#ifdef DEVELOPER_BUILD
    dummy_exec.terminate();
    dummy_exec.waitForFinished();
#endif

    return result;
}
