
#include "../system.hpp"

#include <QStandardPaths>

QString System::getApplicationConfigDir()
{
    QString result = QStandardPaths::displayName(QStandardPaths::HomeLocation);

    result += "/.config/photo_broom";

    return result;
}


std::string System::userName()
{
    const QString name = qgetenv("USER");

    return name.toStdString();
}

