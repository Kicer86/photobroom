
#include "../system.hpp"

#include <QStandardPaths>


std::string System::findProgram(const std::string& name)
{
    QString path = QStandardPaths::findExecutable(name.c_str());
    
    return path.toStdString();
}
