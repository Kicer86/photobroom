
#include "../system.hpp"

std::string System::userName()
{
    const QString name = qgetenv("USERNAME");

    return name.toStdString();
}
