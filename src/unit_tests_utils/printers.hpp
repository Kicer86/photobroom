
#ifndef PRINTERS_HPP
#define PRINTERS_HPP

#include <QString>

inline void PrintTo(const QString& qstring, ::std::ostream* os)
{
    *os << qstring.toStdString();
}

#endif
