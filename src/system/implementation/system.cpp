
#include "../system.hpp"

#include <mutex>

#include <QCoreApplication>
#include <QDir>
#include <QStandardPaths>
#include <QTemporaryDir>


namespace
{
    struct TmpDir: public ITmpDir
    {
        TmpDir(const QString& base_dir, const QString& name):
            m_dir()
        {
            // create sub dir
            for(int c = 0;; c++)
            {
                const QString sub_path = QString("%2%3")
                                            .arg(name)
                                            .arg(c++);
                QDir cd(base_dir);
                const bool status = cd.mkdir(sub_path);

                if (status)
                {
                    m_dir = QDir(base_dir + "/" + sub_path);
                    break;
                }
            }
        }

        ~TmpDir()
        {
            m_dir.removeRecursively();
        }

        QString path() const override
        {
            return m_dir.path();
        }

    private:
        QDir m_dir;
    };

    std::mutex g_dir_creation;
    std::map<QString, std::unique_ptr<ITmpDir>> g_persistentTmps;
}


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


QString System::getTempFilePatternFor(const QString& extension)
{
    const QString tmp_name = QDir::tempPath() + "/" +
                             QCoreApplication::applicationName() +
                             "-XXXXXX." + extension;

    return tmp_name;
}


std::string System::findProgram(const std::string& name)
{
    const QString path = QStandardPaths::findExecutable(name.c_str());

    return path.toStdString();
}


std::unique_ptr<ITmpDir> System::getTmpDir(const QString& utility)
{
    std::unique_lock<std::mutex> l(g_dir_creation);

    const QString base = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    const QString wd = "working_dir";
    const QDir base_dir(base);

    if (base_dir.exists(wd) == false)
        base_dir.mkdir(wd);

    const QString full = base + "/" + wd;

    return std::make_unique<TmpDir>(full, utility);
}


ITmpDir* System::persistentTmpDir(const QString& utility)
{
    auto it = g_persistentTmps.find(utility);

    if (it == g_persistentTmps.end())
    {
        auto i_it = g_persistentTmps.emplace(utility, getTmpDir(utility));

        it = i_it.first;
    }

    return it->second.get();
}


QString System::getTmpFile(ITmpDir* dir, const QString& fileExt)
{
    static int v = 0;

    const QString path = dir->path();

    QFile f;
    for(;;)
    {
        const QString full_path = QString("%1/%2.%3")
                                    .arg(path)
                                    .arg(v++, 6, 16, QLatin1Char('0'))
                                    .arg(fileExt);

        f.setFileName(full_path);
        const bool s = f.open(QIODevice::NewOnly);

        if (s)
            break;
    }

    return f.fileName();
}


void System::cleanTemporaries()
{
    std::unique_lock<std::mutex> l(g_dir_creation);

    const QString base = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    const QString wd = "working_dir";
    const QString full = base + "/" + wd;

    QDir base_dir(full);
    base_dir.removeRecursively();
}
