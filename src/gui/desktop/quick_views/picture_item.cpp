
#include <QPainter>

#include "picture_item.hpp"


PictureItem::PictureItem(QQuickItem* p)
    : QQuickPaintedItem(p)
    , m_scale(1.0)
{
}


void PictureItem::setSource(const QImage& image)
{
    m_source = image;
    prepareSource();
}


void PictureItem::setPictureScale(double pic_scale)
{
    QSize size = m_source.size();
    size *= pic_scale;

    if (size.width() > 100 && size.height() > 100 && pic_scale < 8)
    {
        m_scale = pic_scale;

        prepareSource();
    }
}


const QImage& PictureItem::source() const
{
    return m_source;
}


double PictureItem::pictureScale() const
{
    return m_scale;
}


void PictureItem::paint(QPainter* painter)
{
    QRectF rect(QPointF(0, 0), size());
    painter->drawImage(rect, m_processedSource);
}


void PictureItem::prepareSource()
{
    QSize size = m_source.size();
    size *= m_scale;

    m_processedSource = m_source.scaled(size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    setSize(size);
}
