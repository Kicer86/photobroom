
#include "file_info.hpp"

#include <QFileInfo>


QString FileInfo::fullFilePath(const QString& path)
{
    return QFileInfo(path).absoluteFilePath();
}


QString FileInfo::fullDirectoryPath(const QString& path)
{
    const QString dir = QFileInfo(path).absolutePath();

    return dir;
}


QUrl FileInfo::toUrl(const QString& path)
{
    return QUrl::fromLocalFile(path);
}
