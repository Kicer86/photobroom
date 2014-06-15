
#include "../system.hpp"

#include <assert.h>
#include <string>
#include <cstdlib>

namespace
{
    std::string readVar(const char* name)
    {
        char* result = nullptr;
        size_t size = 0;

        _dupenv_s(&result, &size, name);

        return std::string(result);
    }
}


std::string System::getApplicationConfigDir()
{
    std::string result(readVar("APPDATA"));

    result += "/broom";

    return result;
}


std::string System::findProgram(const std::string &)
{
    //TODO: use "where"
    assert(!"Not implemented");

    return "";
}


std::string System::userName()
{
    const std::string result(readVar("USERNAME"));

    return result;
}
