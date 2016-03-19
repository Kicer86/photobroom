
#include <iostream>

#include <QApplication>
#include <QDebug>
#include <QTranslator>

#include "system/filesystem.hpp"

#include "ui/crash_dialog.hpp"
#include "command_line_parser.hpp"
#include "debugger_factory.hpp"


int main(int argc, char** argv)
{
    int result = 1;

    QApplication app(argc, argv);
    QApplication::setApplicationName("CrashDialog");
    QApplication::setApplicationVersion("1.0");

    const QString tr_path = FileSystem().getTranslationsPath();
    qDebug().noquote() << "Translations path: " << tr_path;

    QTranslator translator;
    translator.load("crash_catcher_dialog_pl", tr_path);
    const bool status = QCoreApplication::installTranslator(&translator);

    if (status)
        qDebug().noquote() << "Polish translations loaded successfully.";
    else
        qWarning().noquote() << "Could not load Polish translations.";

    CommandLineParser parser(app);

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

    return result;
}
