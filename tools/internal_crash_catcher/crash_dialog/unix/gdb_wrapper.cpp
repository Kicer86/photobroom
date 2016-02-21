
#include "gdb_wrapper.hpp"


GDBWrapper::GDBWrapper()
{

}


bool GDBWrapper::attach(qint64 pid, qint64 tid)
{
    return true;
}


std::vector<std::string> GDBWrapper::getBackTrace()
{
    return std::vector<std::string>();
}
