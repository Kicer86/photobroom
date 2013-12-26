
#include "../system.hpp" 

#include <stdlib.h>

std::string System::getApplicationConfigDir()
{
   std::string result(getenv("HOME"));
   
   result += "/.config/broom";
   
   return result;
}
