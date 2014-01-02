
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
    //http://stackoverflow.com/questions/11587426/get-current-username-in-c-on-windows
    static_assert(false, "Not implemented");
}
