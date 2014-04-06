
#include "../system.hpp"

#include <string>
#include <cstdlib>

std::string System::getApplicationConfigDir()
{
   std::string result(getenv("APPDATA"));

   result += "/broom";

   return result;
}


std::string System::findProgram(const std::string &)
{
    //TODO: use "where"
    static_assert(false, "Not implemented");
}


std::string System::userName()
{
    std::string result(getenv("USERNAME"));

    return result;
}
