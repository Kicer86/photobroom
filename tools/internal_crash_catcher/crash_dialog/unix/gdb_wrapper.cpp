
#include "gdb_wrapper.hpp"


GDBWrapper::GDBWrapper(const QString& path)
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
