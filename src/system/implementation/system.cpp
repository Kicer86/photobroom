
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


QString System::getTempFilePath()
{
    static int v = 0;

    const QString result = QString("%1/%2").arg(getApplicationTempDir()).arg(v++, 6, 16, QLatin1Char('0'));

    return result;
}


std::string System::findProgram(const std::string& name)
{
    const QString path = QStandardPaths::findExecutable(name.c_str());

    return path.toStdString();
}
