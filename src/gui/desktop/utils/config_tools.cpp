
#include "config_tools.hpp"

namespace ConfigTools
{
    QColor intToColor(uint32_t c)
    {
        const int r = (c >> 24) & 0xff;
        const int g = (c >> 16) & 0xff;
        const int b = (c >> 8)  & 0xff;
        const int a = c & 0xff;

        return QColor(r, g, b, a);
    }


    uint32_t colorToInt(const QColor& c)
    {
        uint32_t result = 0;

        result |= (c.red() & 0xffu) << 24;
        result |= (c.green() & 0xff) << 16;
        result |= (c.blue() & 0xff) << 8;
        result |= (c.alpha() & 0xff);

        return result;
    }

}
