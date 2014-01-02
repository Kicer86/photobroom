
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
    static_assert(false, "Not implemented");
}


std::string System::userName()
{
    static_assert(false, "Not implemented");
}
