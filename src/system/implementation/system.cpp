
#include "../system.hpp"

#include <mutex>

#include <QCoreApplication>
#include <QStandardPaths>
#include <QTemporaryDir>


namespace
{
    struct TmpDir: public ITmpDir
    {
        TmpDir(const std::shared_ptr<QTemporaryDir>& base_dir, const QString& name):
            m_dir(),
            m_tmp_dir(base_dir)
        {
            const QString path = m_tmp_dir->path();

            // create sub dir
            for(int c = 0;; c++)
            {
                const QString sub_path = QString("%2%3")
                                            .arg(name)
                                            .arg(c++);
                QDir cd(path);
                const bool status = cd.mkdir(sub_path);

                if (status)
                {
                    m_dir = QDir(path + "/" + sub_path);
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
        std::shared_ptr<QTemporaryDir> m_tmp_dir;
    };

    std::shared_ptr<QTemporaryDir> g_heavy_dir;
    std::shared_ptr<QTemporaryDir> g_light_dir;

    std::mutex g_dir_creation;
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

    if (g_light_dir.get() == nullptr)
        g_light_dir = std::make_shared<QTemporaryDir>();    // temporary dir for light files was not created yet, make one

    return std::make_unique<TmpDir>(g_light_dir, utility);
}


std::unique_ptr<ITmpDir> System::getHeavyTmpDir(const QString& utility)
{
}
