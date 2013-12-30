
#ifndef __SYSTEM_HPP__
#define __SYSTEM_HPP__

#include <string>

#include "system_export.h"


struct SYSTEM_EXPORT System
{
    static std::string getApplicationConfigDir();
    static std::string findProgram(const std::string& name);
};


#endif
