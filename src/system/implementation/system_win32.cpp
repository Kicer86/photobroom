
module;

#include <QString>
#include <string>

export module system:win32_system;
import :system;


std::string System::userName()
{
    const QString name = qgetenv("USERNAME");

    return name.toStdString();
}
