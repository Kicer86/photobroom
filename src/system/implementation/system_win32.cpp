
module;

#include <cstdlib>
#include <string>

module broom.system;
import :system;


std::string System::userName()
{
    const auto name = std::getenv("USERNAME");

    return name;
}
