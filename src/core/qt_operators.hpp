
#ifndef QT_OPERATORS_HPP_INCLUDED
#define QT_OPERATORS_HPP_INCLUDED

#include <tuple>
#include <QRect>
#include <QString>

inline auto operator<=>(const QRect& lhs, const QRect& rhs)
{
    return std::make_tuple(lhs.x(), lhs.y(), lhs.width(), lhs.height()) <=> std::make_tuple(rhs.x(), rhs.y(), rhs.width(), rhs.height());
}

#if QT_VERSION < QT_VERSION_CHECK(6, 8, 0)
inline auto operator<=>(const QString& lhs, const QString& rhs)
{
    if (lhs < rhs)
        return std::strong_ordering::less;
    else if (lhs > rhs)
        return std::strong_ordering::greater;
    else
        return std::strong_ordering::equal;
}
#endif

#endif
