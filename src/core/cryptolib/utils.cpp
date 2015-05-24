
#include <iomanip>

std::string format(unsigned char* raw, int len)
{
    std::stringstream ss;

    for(int i = 0; i < len; i++)
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(raw[i]);

    return ss.str();
}
