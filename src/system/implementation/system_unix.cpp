
#include "../system.hpp"

#include <stdlib.h>

#include <QRegExp>
#include <QProcess>

namespace
{
    std::string run(const std::string& program)
    {
        QProcess process;

        process.closeWriteChannel();
        process.start(program.c_str(), QProcess::ReadOnly);
        process.waitForFinished(3000);

        const QString line = process.readAll();
        const std::string result = line.toStdString();

        return result;
    }
}


QString System::getApplicationConfigDir()
{
    QString result(getenv("HOME"));

    result += "/.config/broom";

    return result;
}


std::string System::findProgram(const std::string& name)
{
    const std::string result = run("whereis " + name);
    const QString pattern = QString("%1: ([^ ]+).*").arg(name.c_str());
    const QRegExp regex(pattern);
    const bool matches = regex.exactMatch(result.c_str());

    std::string path;

    if (matches)
        path = regex.capturedTexts()[1].toStdString();

    return path;
}


std::string System::userName()
{
    const std::string result = run("whoami");

    QString name(result.c_str());
    name = name.simplified();

    return name.toStdString();
}

