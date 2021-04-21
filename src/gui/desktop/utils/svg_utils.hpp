
#ifndef SVG_UTILS_HPP_INCLUDED
#define SVG_UTILS_HPP_INCLUDED

#include <QImage>


namespace SVGUtils
{
    QImage load(const QString& path, const QSize& size);
}

#endif // SVG_UTILS_HPP_INCLUDED
