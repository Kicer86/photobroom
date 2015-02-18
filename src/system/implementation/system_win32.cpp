
#include "../system.hpp"

#include <cassert>
#include <cstdlib>

#include <QStandardPaths>

QString System::getApplicationConfigDir()
{
    const QString result = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

    return result;
}


std::string System::userName()
{
    const QString name = qgetenv("USERNAME");

    return name.toStdString();
}
