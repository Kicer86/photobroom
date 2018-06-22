
#include <iostream>

#include <QCoreApplication>
#include <QProcess>
#include <QStandardPaths>

// Expected arguments:
// 1. output path
// 2. all binaries which use Qt


int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);

    const QString qtwindeploy = QStandardPaths::findExecutable("qtwindeploy");
    if (qtwindeploy.isEmpty())
    {
        std::cerr << "Could not find qtwindeploy in PATH" << std::endl;
        return 1;
    }

    QStringList args = app.arguments();
    const QString out = args.takeFirst();
    const QStringList bins = args;

    for (const QString& bin: bins)
    {
        const QStringList dep_args = 
        {
            "--dir", out + "/tr",
            "--libdir", out + "/lib",
            "--no-compiler-runtime",
            "--release",
            bin
        };

        QProcess::execute(qtwindeploy, dep_args);
    }

    return app.exec();
}
