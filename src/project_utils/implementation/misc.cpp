
#include "misc.hpp"

#include <filesystem>
#include <QDataStream>
#include <QDir>
#include <QFileInfo>
#include <QTemporaryFile>

#include "project.hpp"
#include <system/system.hpp>


namespace
{
    std::filesystem::path prepareDestinationPath(const ProjectInfo& prjInfo, const QString& path)
    {
        const QFileInfo originalFileInfo(path);
        const QString extension = originalFileInfo.completeSuffix();
        const QString mediaLocation = prjInfo.getInternalLocation(ProjectInfo::PrivateMultimedia);
        const QString uniqueFileName = System::getUniqueFileName(mediaLocation, extension);
        const QFileInfo uniqueFileInfo(uniqueFileName);

        return uniqueFileInfo.filesystemAbsoluteFilePath();
    }

    std::filesystem::path prepareSourcePath(const ProjectInfo& prjInfo, const QString& path)
    {
        const QString mediaLocation = prjInfo.getInternalLocation(ProjectInfo::PrivateMultimedia);
        const QFileInfo pathInfo(path);
        const QDir mediaDir(mediaLocation);
        const QString relativePath = mediaDir.relativeFilePath(pathInfo.absoluteFilePath());

        return std::filesystem::path(relativePath.toStdString());;
    }

    QString moveFileToPrivateMediaLocation(const ProjectInfo& prjInfo, const QString& path)
    {
        const std::filesystem::path uniqueFileName = prepareDestinationPath(prjInfo, path);
        const QFileInfo pathInfo(path);
        const std::filesystem::path sourcePath = pathInfo.filesystemAbsoluteFilePath();

        std::filesystem::rename(sourcePath, uniqueFileName);

        return uniqueFileName.c_str();
    }

    QString linkFileToPrivateMediaLocation(const ProjectInfo& prjInfo, const QString& path)
    {
        const std::filesystem::path uniqueFileName = prepareDestinationPath(prjInfo, path);
        const std::filesystem::path sourcePath = prepareSourcePath(prjInfo, path);

        std::filesystem::create_symlink(sourcePath, uniqueFileName);

        return uniqueFileName.c_str();
    }
}


QString includeFileInPrivateMediaLocation(const ProjectInfo& prjInfo, const QString& path)
{
    return path.left(5) == "prj:/"?
        linkFileToPrivateMediaLocation(prjInfo, path):
        moveFileToPrivateMediaLocation(prjInfo, path);
}
