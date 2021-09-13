
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
    std::filesystem::path prepareDesitnationPath(const ProjectInfo& prjInfo, const QString& path)
    {
        const QFileInfo originalFileInfo(path);
        const QString extension = originalFileInfo.completeSuffix();
        const QString mediaLocation = prjInfo.getInternalLocation(ProjectInfo::PrivateMultimedia);
        const QString uniqueFileName = System::getUniqueFileName(mediaLocation, extension);
        QFile::remove(uniqueFileName);          // remove destination file

        const QFileInfo uniqueFileInfo(uniqueFileName);
        return uniqueFileInfo.filesystemAbsoluteFilePath();
    }
}


QString moveFileToPrivateMediaLocation(const ProjectInfo& prjInfo, const QString& path)
{
    const std::filesystem::path uniqueFileName = prepareDesitnationPath(prjInfo, path);
    const QFileInfo pathInfo(path);
    const std::filesystem::path sourcePath = pathInfo.filesystemAbsoluteFilePath();

    std::filesystem::rename(sourcePath, uniqueFileName);

    return uniqueFileName.c_str();
}



QString linkFileToPrivateMediaLocation(const ProjectInfo& prjInfo, const QString& path)
{
    const std::filesystem::path uniqueFileName = prepareDesitnationPath(prjInfo, path);
    const QFileInfo pathInfo(path);
    const std::filesystem::path sourcePath = pathInfo.filesystemAbsoluteFilePath();

    std::filesystem::create_symlink(sourcePath, uniqueFileName);

    return uniqueFileName.c_str();
}
