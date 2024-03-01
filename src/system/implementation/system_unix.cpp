
module;

#include <QString>
#include <string>

export module system:unix_system;
import :system;


std::string System::userName()
{
    const QString name = qgetenv("USER");

    return name.toStdString();
}
