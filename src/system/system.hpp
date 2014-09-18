
#ifndef __SYSTEM_HPP__
#define __SYSTEM_HPP__

#include <QString>

#include "system_export.h"


struct SYSTEM_EXPORT System
{
    static QString getApplicationConfigDir();
    static std::string findProgram(const std::string& name);
    static std::string userName();
};


#endif
