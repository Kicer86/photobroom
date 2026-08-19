
#include "misc.hpp"

#include <QDataStream>
#include <QDir>
#include <QFileInfo>
#include <QTemporaryFile>

#include "project.hpp"

import broom.system;

// TODO: REWRITE

/*
namespace
{
    Filesystem::Location prepareDestinationPath(const ProjectInfo& prjInfo, const QString& path)
    {
        const QFileInfo originalFileInfo(path);
        const QString extension = originalFileInfo.completeSuffix();
        const QString mediaLocation = prjInfo.getInternalLocation(ProjectInfo::PrivateMultimedia);
        const QString uniqueFileName = System::getUniqueFileName(mediaLocation, extension);
        const QFileInfo uniqueFileInfo(uniqueFileName);

        return Filesystem::Location(uniqueFileInfo.filesystemAbsoluteFilePath());
    }

    Filesystem::Location prepareSourcePath(const ProjectInfo& prjInfo, const QString& path)
    {
        const QString mediaLocation = prjInfo.getInternalLocation(ProjectInfo::PrivateMultimedia);
        const QFileInfo pathInfo(path);
        const QDir mediaDir(mediaLocation);
        const QString relativePath = mediaDir.relativeFilePath(pathInfo.absoluteFilePath());

        return Filesystem::Location(relativePath);
    }

    Filesystem::Location moveFileToPrivateMediaLocation(const ProjectInfo& prjInfo, const QString& path)
    {
        const Filesystem::Location uniqueFileName = prepareDestinationPath(prjInfo, path);
        const QFileInfo pathInfo(path);
        const std::filesystem::path sourcePath = pathInfo.filesystemAbsoluteFilePath();

        std::filesystem::copy(sourcePath, uniqueFileName);

        return QString::fromStdString(uniqueFileName.string());
    }

    Filesystem::Location linkFileToPrivateMediaLocation(const ProjectInfo& prjInfo, const QString& path)
    {
        const Filesystem::Location uniqueFileName = prepareDestinationPath(prjInfo, path);
        const Filesystem::Location sourcePath = prepareSourcePath(prjInfo, path);

        std::filesystem::create_symlink(sourcePath, uniqueFileName);

        return uniqueFileName;
    }
}
*/


Filesystem::Location includeFileInPrivateMediaLocation(const ProjectInfo& prjInfo, const Filesystem::Location& location)
{
    /*
    const auto path = location.url();

    return path.left(5) == "prj:/"?
        linkFileToPrivateMediaLocation(prjInfo, path):
        moveFileToPrivateMediaLocation(prjInfo, path);
    */

    return {};
}
