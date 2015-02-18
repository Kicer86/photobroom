
#include "../system.hpp"

#include <QStandardPaths>



QString System::getApplicationConfigDir()
{
    const QString result = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

    return result;
}


std::string System::findProgram(const std::string& name)
{
    QString path = QStandardPaths::findExecutable(name.c_str());
    
    return path.toStdString();
}
