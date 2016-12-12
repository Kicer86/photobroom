
#include "../system.hpp"

#include <QStandardPaths>
#include <QTemporaryDir>



QString System::getApplicationConfigDir()
{
    const QString result = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

    return result;
}


QString System::getApplicationTempDir()
{
    static QTemporaryDir tmpDir;

    return tmpDir.path();
}


std::string System::findProgram(const std::string& name)
{
    const QString path = QStandardPaths::findExecutable(name.c_str());

    return path.toStdString();
}
