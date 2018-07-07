
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
    static std::unique_ptr<ITmpDir> getTmpDir(const QString& utility);       // Returns dir in tmp dir for given utility. For small files only
    static std::unique_ptr<ITmpDir> getHeavyTmpDir(const QString& utility);  // Returns dir in tmp dir for given utility. For big files
};


#endif
