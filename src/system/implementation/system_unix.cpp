
#include "../system.hpp"


std::string System::userName()
{
    const QString name = qgetenv("USER");

    return name.toStdString();
}
