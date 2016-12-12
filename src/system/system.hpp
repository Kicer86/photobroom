
#ifndef SYSTEM_HPP
#define SYSTEM_HPP

#include <QString>

#include "system_export.h"


struct SYSTEM_EXPORT System
{
    static QString getApplicationConfigDir();
    static QString getApplicationTempDir();
    static QString getTempFilePath();
    static std::string findProgram(const std::string& name);
    static std::string userName();
};


#endif
