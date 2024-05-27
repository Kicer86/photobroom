
module;

#include <cstdlib>
#include <string>

export module system:unix_system;
import :system;


std::string System::userName()
{
    const auto name = std::getenv("USER");

    return name;
}
