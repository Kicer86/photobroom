
#include <QApplication>

#include "ui/crash_dialog.hpp"
#include "debugger_factory.hpp"

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    DebuggerFactory dbgFactory;
    std::unique_ptr<IDebugger> debugger = dbgFactory.get();

    CrashDialog dialog(debugger.get());
    dialog.show();

    const int result = app.exec();

    return result;
}
