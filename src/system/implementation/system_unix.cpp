
#include "../system.hpp"

#include <stdlib.h>

#include <QRegExp>
#include <QProcess>


QString System::getApplicationConfigDir()
{
    QString result(getenv("HOME"));

    result += "/.config/broom";

    return result;
}


std::string System::userName()
{
    const QString name = qgetenv("USER");

    return name.toStdString();
}

