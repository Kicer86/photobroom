
#include <QPainter>

#include "picture_item.hpp"


PictureItem::PictureItem(QQuickItem* p)
    : QQuickPaintedItem(p)
{
}


void PictureItem::setSource(const QImage& image)
{
    m_source = image;

    setSize(image.size());
}


const QImage & PictureItem::source() const
{
    return m_source;
}


void PictureItem::paint(QPainter* painter)
{
    QRectF rect(QPointF(0, 0), size());
    painter->drawImage(rect, m_source);
}
