
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
    enum TmpOptions
    {
        Confidential = 1,
        Persistent   = 2,
        BigFiles     = 4,
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
     * \param utility temporary dir purpose. It will be used as a part of subdir in temporary directory.fgfgbghn
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
     * \brief remove all temporary files
     */
    static void cleanTemporaries();
};


#endif
