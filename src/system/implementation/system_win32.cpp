
module;

#include <cstdlib>
#include <string>

module system;
import :system;


std::string System::userName()
{
    const auto name = std::getenv("USERNAME");

    return name;
}
