
#include "../system.hpp"

#include <cassert>
#include <cstdlib>

#include <QStandardPaths>


std::string System::userName()
{
    const QString name = qgetenv("USERNAME");

    return name.toStdString();
}
