
#include "../system.hpp"

#include <mutex>

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QStandardPaths>
#include <QTemporaryDir>


namespace
{
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

    std::mutex g_dir_creation;
    std::map<QString, std::shared_ptr<ITmpDir>> g_persistentTmps;
    QString g_systemTmp;
}


QString System::getApplicationConfigDir()
{
    const QString result = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

    return result;
}


std::shared_ptr<ITmpDir> System::getTmpDir(const QString& utility)
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


std::shared_ptr<ITmpDir> System::persistentTmpDir(const QString& utility)
{
    auto it = g_persistentTmps.find(utility);

    if (it == g_persistentTmps.end())
    {
        auto i_it = g_persistentTmps.emplace(utility, getTmpDir(utility));

        it = i_it.first;
    }

    return it->second;
}


QString System::getTmpFile(const QString& path, const QString& fileExt)
{
    static int v = 0;

    QString result;

    for(;;)
    {
        result = QString("%1/%2.%3")
                    .arg(path)
                    .arg(v++, 6, 16, QLatin1Char('0'))
                    .arg(fileExt);

        // TODO: use QIODevice::NewOnly with QFile in future (when Qt5.11 is more popular)
        // see commit ef8d5f8dfd8cde2138525101aac4fb1ad52dc864

        if (QFile::exists(result) == false)
        {
            QFile c(result);
            c.open(QIODevice::WriteOnly);
            break;
        }
    }

    return result;
}


std::shared_ptr<ITmpDir> System::getSysTmpDir(const QString& utility)
{
    std::unique_lock<std::mutex> l(g_dir_creation);

    if (g_systemTmp.isEmpty())
    {
        const QString base = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
        const QString name = QCoreApplication::applicationName();
        g_systemTmp = createUniqueDir(base, name);
    }

    return std::make_shared<TmpDir>(g_systemTmp, utility);
}


std::shared_ptr<ITmpDir> System::createTmpDir(const QString& utility, TmpOptions flags)
{
    if (flags == Confidential)
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
    else if (flags == Persistent)
    {
        auto it = g_persistentTmps.find(utility);

        if (it == g_persistentTmps.end())
        {
            auto i_it = g_persistentTmps.emplace(utility, getTmpDir(utility));

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
