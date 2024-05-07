
#ifndef PRINTERS_HPP
#define PRINTERS_HPP

#include <iostream>

#include <QModelIndex>
#include <QRect>
#include <QSize>
#include <QString>


template<typename T>
concept isQtPrintable = requires(QDebug d, T a)
{
    { d << a };
};


template<typename T>
requires isQtPrintable<T>
inline void PrintTo(const T& qtObj, std::ostream* os)
{
    QString qtStr;
    QDebug debug(&qtStr);
    debug << qtObj;

    *os << qtStr.toStdString();
}

#endif
