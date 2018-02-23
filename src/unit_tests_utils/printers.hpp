
#ifndef PRINTERS_HPP
#define PRINTERS_HPP

#include <iostream>

#include <QModelIndex>
#include <QRect>
#include <QSize>
#include <QString>

inline void PrintTo(const QString& qstring, ::std::ostream* os)
{
    *os << qstring.toStdString();
}

inline void PrintTo(const QPoint& qpoint, std::ostream* os)
{
    *os << "x = " << qpoint.x() << ", y = " << qpoint.y();
}

inline void PrintTo(const QSize& qsize, std::ostream* os)
{
    *os << "width = " << qsize.width() << ", height = " << qsize.height();
}

inline void PrintTo(const QRect& qrect, std::ostream* os)
{
    PrintTo(qrect.topLeft(), os);
    *os << ", ";
    PrintTo(qrect.size(), os);
}

inline void PrintTo(const QModelIndex& qmodelindex, std::ostream* os)
{
    *os << "row = " << qmodelindex.row() << ", column = " << qmodelindex.column();
    *os << ", model = " << qmodelindex.model() << ", internalId = " << qmodelindex.internalId();
}

#endif
