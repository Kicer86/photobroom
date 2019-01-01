
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
    static QString getApplicationConfigDir();
    [[deprecated]] static QString getApplicationTempDir();
    [[deprecated]] static QString getTempFilePath();
    [[deprecated]] static QString getTempFilePatternFor(const QString& extension);  // just xyz without a dot
    static std::string findProgram(const std::string& name);         // TODO: is is really necessary?
    static std::string userName();

    // Group of functions returning temporary files/dir created in user's home dir.
    // Meant for heavy or fragile data
    static std::unique_ptr<ITmpDir> getTmpDir(const QString& utility);       // Returns dir in tmp dir for given utility. Removed when ITmpDir is deleted
    static ITmpDir* persistentTmpDir(const QString& utility);                // Returns dir in tmp dir for given utility. Removed when application is closed.

    //
    static QString getTmpFile(const QString& dir, const QString& fileExt);   // Returns unique file name with given extension in given tmp dir

    // Group of function returning temporary directories in system temporary dir (possibly ram disk)
    // Meant for small or often changing files.
    static std::unique_ptr<ITmpDir> getSysTmpDir(const QString& utility);    // Returns dir in tmp dir for given utility. Removed when ITmpDir is deleted

    //
    static void cleanTemporaries();                                          // removes all temporary files
};


#endif
