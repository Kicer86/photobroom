
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
    QString prepareDesitnationPath(const ProjectInfo& prjInfo, const QString& path)
    {
        const QFileInfo originalFileInfo(path);
        const QString extension = originalFileInfo.completeSuffix();
        const QString mediaLocation = prjInfo.getInternalLocation(ProjectInfo::PrivateMultimedia);
        const QString uniqueFileName = System::getUniqueFileName(mediaLocation, extension);
        QFile::remove(uniqueFileName);          // std::filesystem::copy() is not happy when desitnation file exists
                                                // even when using std::filesystem::copy_options::overwrite_existing flag

        return uniqueFileName;
    }
}


QString moveFileToPrivateMediaLocation(const ProjectInfo& prjInfo, const QString& path)
{
    const QString uniqueFileName = prepareDesitnationPath(prjInfo, path);

    std::filesystem::rename(path.toStdString(),
                            uniqueFileName.toStdString());

    return uniqueFileName;
}



QString linkFileToPrivateMediaLocation(const ProjectInfo& prjInfo, const QString& path)
{
    const QString uniqueFileName = prepareDesitnationPath(prjInfo, path);

    std::filesystem::copy(path.toStdString(),
                          uniqueFileName.toStdString(),
                          std::filesystem::copy_options::overwrite_existing |
                          std::filesystem::copy_options::create_symlinks);

    return uniqueFileName;
}
