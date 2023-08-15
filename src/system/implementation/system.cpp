
#include "../system.hpp"

#include <mutex>

#include <QCache>
#include <QCoreApplication>
#include <QDir>
#include <QStandardPaths>
#include <QTemporaryDir>


namespace
{

    std::mutex g_dir_creation;
    std::map<QString, std::shared_ptr<ITmpDir>> g_persistentTmps;
    QString g_systemTmp;


    QString createUniqueDir(const QString& base_dir, const QString& name)
    {
        // create sub dir
        QDir result;

        for(int c = 0;; c++)
        {
            const QString sub_path = QString("%2%3")
                                        .arg(name)
                                        .arg(c);
            QDir cd(base_dir);
            const bool status = cd.mkdir(sub_path);

            if (status)
            {
                result = QDir(base_dir + "/" + sub_path);
                break;
            }
        }

        return result.path();
    }


    struct TmpDir: public ITmpDir
    {
        TmpDir(const QString& base_dir, const QString& name):
            m_dir(createUniqueDir(base_dir, name))
        {

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

    std::unique_ptr<ITmpDir> createUserHomeDirTmpDir(const QString& utility)
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

    std::unique_ptr<ITmpDir> createSystemLevelTmpDir(const QString& utility)
    {
        std::unique_lock<std::mutex> l(g_dir_creation);

        if (g_systemTmp.isEmpty())
        {
            const QString base = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
            const QString name = QCoreApplication::applicationName();
            g_systemTmp = createUniqueDir(base, name);
        }

        return std::make_unique<TmpDir>(g_systemTmp, utility);
    }
}


QString System::getApplicationConfigDir()
{
    const QString result = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

    return result;
}


QString System::getUniqueFileName(const QString& path, const QString& fileExt)
{
    static QCache<QString, int> state(10);

    int* cachedValue = state.object(path);
    int value = cachedValue == nullptr?
        static_cast<int>(QDir(path).entryList().size()):
        *cachedValue;

    QString full_path;

    for(;;)
    {
        full_path = QString("%1/%2.%3")
                        .arg(path)
                        .arg(value++, 8, 16, QLatin1Char('0'))
                        .arg(fileExt);

        const QFileInfo fileInfo(full_path);
        const bool exists = fileInfo.isSymLink() || fileInfo.exists();

        if (exists == false)
            break;
    }

    state.insert(full_path, new int(value));

    return full_path;
}


std::shared_ptr<ITmpDir> System::createTmpDir(const QString& utility, QFlags<TmpOption> flags)
{
    // Either only Generic was set or anything but Generic.
    assert(flags == Generic || ((flags & Generic) == 0));

    // Generic - no big files or sensistive data, put stuff in system temporary location which may be in RAM.
    if (flags == Generic)
        return createSystemLevelTmpDir(utility);
    // Confidental or big data - store in use home dir
    else if ((flags & Confidential) || (flags & BigFiles))
        return createUserHomeDirTmpDir(utility);
    else if (flags & Persistent)
    {
        auto it = g_persistentTmps.find(utility);

        if (it == g_persistentTmps.end())
        {
            auto i_it = g_persistentTmps.emplace(utility, createUserHomeDirTmpDir(utility));

            it = i_it.first;
        }

        return it->second;
    }

    assert(!"unhandled flags");
    return nullptr;
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
