
#include "../system.hpp"

#include <QStandardPaths>



QString System::getApplicationConfigDir()
{
    QString result = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

    result += "/photo_broom";

    return result;
}


std::string System::findProgram(const std::string& name)
{
    const QString path = QStandardPaths::findExecutable(name.c_str());
    
    return path.toStdString();
}
