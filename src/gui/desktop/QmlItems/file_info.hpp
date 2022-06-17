
#ifndef FILE_INFO_HPP_INCLUDED
#define FILE_INFO_HPP_INCLUDED

#include <QObject>
#include <QtQmlIntegration>


class FileInfo: public QObject
{
    Q_OBJECT

public:
    Q_INVOKABLE static QString fullFilePath(const QString &);
    Q_INVOKABLE static QString fullDirectoryPath(const QString &);
    Q_INVOKABLE static QUrl    toUrl(const QString &);
};

#endif
