
#include <QPainter>

#include "picture_item.hpp"


PictureItem::PictureItem(QQuickItem* p)
    : QQuickPaintedItem(p)
{

}


void PictureItem::setSource(const QImage& image)
{
    m_source = image;

    const QSize img_size = m_source.size();

    setImplicitWidth(img_size.width());
    setImplicitHeight(img_size.height());

    update();

    emit sourceChanged();
}


const QImage& PictureItem::source() const
{
    return m_source;
}


void PictureItem::paint(QPainter* painter)
{
    const QRectF rect(QPointF(0, 0), QSizeF(width(), height()));
    painter->drawImage(rect, m_source);
}


bool PictureItem::validateInputs() const
{
    const QSize img_size = m_source.size();
    const QSizeF item_size = size();

    return (img_size.isEmpty() || item_size.height() < 1.0 || item_size.width() < 1.0) == false;
}
