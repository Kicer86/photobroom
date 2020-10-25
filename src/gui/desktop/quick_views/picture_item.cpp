
#include <QPainter>

#include "picture_item.hpp"


PictureItem::PictureItem(QQuickItem* p)
    : QQuickPaintedItem(p)
    , m_scale(1.0)
    , m_mode(Mode::ZoomToFit)
{
    listenForResize(true);
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
        m_mode = Mode::FixedScale;

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


void PictureItem::listenForResize(bool enable)
{
    if (enable)
    {
        connect(this, &PictureItem::widthChanged, this, &PictureItem::prepareSource);
        connect(this, &PictureItem::heightChanged, this, &PictureItem::prepareSource);
    }
    else
    {
        disconnect(this, &PictureItem::widthChanged, this, &PictureItem::prepareSource);
        disconnect(this, &PictureItem::heightChanged, this, &PictureItem::prepareSource);
    }
}


bool PictureItem::validateInputs() const
{
    const QSize img_size = m_source.size();
    const QSizeF item_size = size();

    return (img_size.isEmpty() || item_size.height() < 1.0 || item_size.width() < 1.0) == false;
}


void PictureItem::prepareSource()
{
    if (validateInputs() == false)
        return;

    if (m_mode == Mode::ZoomToFit)
        m_scale = calculateZoomToFit();

    QSize size = m_source.size();
    size *= m_scale;

    m_processedSource = m_source.scaled(size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    listenForResize(false);
    setSize(size);
    listenForResize(true);
}


double PictureItem::calculateZoomToFit() const
{
    const QSize img_size = m_source.size();
    const QSizeF item_size = size();
    const QSize img_scaled = img_size.scaled(item_size.width(), item_size.height(), Qt::KeepAspectRatio);

    return static_cast<double>(img_scaled.height()) / img_size.height();
}
