
#include "../system.hpp"

#include <string>
#include <cstdlib>

std::string System::getApplicationConfigDir()
{
   std::string result(getenv("APPDATA"));

   result += "/broom";

   return result;
}
