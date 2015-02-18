
#include "../system.hpp"

#include <cassert>
#include <cstdlib>

#include <QString>

namespace
{
    QString readVar(const char* name)
    {
#ifdef __GNUC__
        const QString result(getenv(name));

        return result;
#else
        char* result = nullptr;
        size_t size = 0;

        _dupenv_s(&result, &size, name);

        return QString(result);
#endif
    }
}


QString System::getApplicationConfigDir()
{
    QString result(readVar("APPDATA"));

    result += "/broom";

    return result;
}


std::string System::userName()
{
    const QString name = qgetenv("USERNAME");

    return name.toStdString();
}
