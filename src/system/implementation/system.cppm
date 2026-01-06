module;

#include <memory>
#include <string>
#include "system_export.h"

class QString;

template <typename Enum>
class QFlags;

export module system:system;

export struct SYSTEM_EXPORT ITmpDir
{
    virtual ~ITmpDir() = default;

    virtual QString path() const = 0;
};

export struct SYSTEM_EXPORT System
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

    using TmpOptions = QFlags<TmpOption>;

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
    static std::shared_ptr<ITmpDir> createTmpDir(const QString& utility, QFlags<TmpOption> flags);

    /**
     * \brief remove all temporary files
     */
    static void cleanTemporaries();
};
