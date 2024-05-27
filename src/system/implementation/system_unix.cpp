
module;

#include <cstdlib>
#include <string>

module system:system;


std::string System::userName()
{
    const auto name = std::getenv("USER");

    return name;
}
