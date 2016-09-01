
#include "ilogger.hpp"

#include <QString>


InfoStream::InfoStream(ILogger* logger): LoggerStream(logger) 
{
    
}


WarningStream::WarningStream(ILogger* logger): LoggerStream(logger) 
{
    
}


ErrorStream::ErrorStream(ILogger* logger): LoggerStream(logger) 
{
    
}


DebugStream::DebugStream(ILogger* logger): LoggerStream(logger) 
{
    
}


std::ostream& operator<<(std::ostream& os, const QString& str)
{
    os << str.toStdString();

    return os;
}
