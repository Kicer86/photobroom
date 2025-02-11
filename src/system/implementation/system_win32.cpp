
module;

#include <cstdlib>
#include <string>

export module system:win32_system;
import :system;


std::string System::userName()
{
    const auto name = std::getenv("USERNAME");

    return name;
}
