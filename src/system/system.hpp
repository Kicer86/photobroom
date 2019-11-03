
#ifndef SYSTEM_HPP
#define SYSTEM_HPP

#include <memory>

#include <QString>

#include "system_export.h"

struct ITmpDir
{
    virtual ~ITmpDir() = default;

    virtual QString path() const = 0;
};


struct SYSTEM_EXPORT System
{
    /**
     * Flags to be used for temporary directory creation.
     */
    enum TmpOptions
    {
        Generic      = 0,       ///< generic temporary dir without any special requirements. This option cannot be combined with others.
        Confidential = 1,       ///< directory will be used for user private data. No system wide tmp dir should be used as a base.
        Persistent   = 2,       ///< use persistent location which will be reused when called for the same util name.
        BigFiles     = 4,       ///< big files will be stored in tmp dir. RAM tmp dirs won't be used.
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
     * \brief get temporary dir
     * \param utility temporary dir purpose. It will be used as a part of subdir in temporary directory.
     * \return shared_ptr with temporary dir details.
     *
     * Returns dir in tmp dir for given utility. Removed when returned ITmpDir is deleted
     */
    static std::shared_ptr<ITmpDir> getTmpDir(const QString& utility);

    /**
     * \brief get temporary dir
     * \param utility temporary dir purpose. It will be used as a part of subdir in temporary directory.
     * \return pointer to temporary dir details
     *
     * Returns dir in tmp dir for given utility. Removed when application is closed.
     */
    static std::shared_ptr<ITmpDir> persistentTmpDir(const QString& utility);

    /**
     * \brief generate unique file name
     * \param directory where file is to be created
     * \param fileExt file extension
     * \return complete file path
     *
     * Returns unique file name with given extension in given tmp dir
     */
    static QString getTmpFile(const QString& dir, const QString& fileExt);

    // Group of function returning temporary directories in system temporary dir (possibly ram disk)
    // Meant for small or often changing files.

    /**
     * \brief create temporary dir in system temporary dir
     * \param utility temporary dir purpose. It will be used as a part of subdir in temporary directory.
     * \return shared_ptr with temporary dir details.
     *
     * Returns dir in tmp dir for given utility. Removed when ITmpDir is deleted
     */
    static std::shared_ptr<ITmpDir> getSysTmpDir(const QString& utility);


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
    static std::shared_ptr<ITmpDir> createTmpDir(const QString& utility, TmpOptions flags = Generic);

    /**
     * \brief remove all temporary files
     */
    static void cleanTemporaries();
};


#endif
