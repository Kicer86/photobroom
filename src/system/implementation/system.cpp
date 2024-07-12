
module;

#include <mutex>

#include <QCache>
#include <QCoreApplication>
#include <QDir>
#include <QStandardPaths>
#include <QTemporaryDir>


export module system:system;


export struct ITmpDir
{
    virtual ~ITmpDir() = default;

    virtual QString path() const = 0;
};


export struct System
{
    /**
     * Flags to be used for temporary directory creation.
     */
    enum TmpOption
    {
        Generic      = 1,       ///< generic temporary dir without any special requirements. This option cannot be combined with others.
        Confidential = 2,       ///< directory will be used for user private data. No system wide tmp dir should be used as a base.
        Persistent   = 4,       ///< use persistent location which will be reused when called for the same util name.
        BigFiles     = 8,       ///< big files will be stored in tmp dir. RAM tmp dirs won't be used.
    };

    /**
     * \brief get dir for config files
     * \return path to directory with conbfiguration files
     */
    static QString getApplicationConfigDir();

    /**
     * \brief get user name
     * \return user name
     */
    static std::string userName();

    // Group of functions returning temporary files/dir created in user's home dir.
    // Meant for heavy or fragile data

    /**
     * \brief generate unique file name
     * \param directory where file is to be created
     * \param fileExt file extension
     * \return complete file path
     *
     * Returns unique file name with given extension in given tmp dir
     */
    static QString getUniqueFileName(const QString& directory, const QString& fileExt);

    /**
     * \brief temporary dir creator
     * \param utility temporary dir purpose. It will be used as a part of subdir in temporary directory.
     * \param flags temporary dir options. Can be combined.
     * \return temporary structure describing temporary dir.
     *
     * Returned structure will keep temporary dir alive.\n
     * When it is destroyed, temporary directory gets deleted.\n
     * Some temporary directories may be shared and won't\n
     * be deleted until last client releases it.
     */
    static std::shared_ptr<ITmpDir> createTmpDir(const QString& utility, QFlags<TmpOption> flags = Generic);

    /**
     * \brief remove all temporary files
     */
    static void cleanTemporaries();

    Q_DECLARE_FLAGS(TmpOptions, TmpOption)
};

Q_DECLARE_OPERATORS_FOR_FLAGS(System::TmpOptions)


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
