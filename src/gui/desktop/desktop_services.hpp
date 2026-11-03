#ifndef DESKTOP_SERVICES_HPP_INCLUDED
#define DESKTOP_SERVICES_HPP_INCLUDED

#include <QDesktopServices>
#include <QFileInfo>
#include <QUrl>

#include <core/filesystem.hpp>


namespace DesktopServices
{
    inline bool openContainingDirectory(const Filesystem::Location& photo)
    {
        const QString localFile = Filesystem::toLocalFile(photo);
        const QFileInfo fileInfo(localFile);

        return QDesktopServices::openUrl(QUrl::fromLocalFile(fileInfo.absolutePath()));
    }
}

#endif
