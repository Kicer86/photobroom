
#ifndef SYSTEM_HPP
#define SYSTEM_HPP

#include <QString>

#include "system_export.h"


struct SYSTEM_EXPORT System
{
    static QString getApplicationConfigDir();
    static QString getApplicationTempDir();
    static QString getTempFilePath();
    static QString getTempFilePatternFor(const QString& extension);  // just xyz without a dot
    static std::string findProgram(const std::string& name);         // TODO: is is really necessary?
    static std::string userName();
};


#endif
