
#ifndef IMAGE_ITEM_HPP
#define IMAGE_ITEM_HPP

#include <QImage>
#include <QQuickPaintedItem>

class PictureItem: public QQuickPaintedItem
{
        Q_OBJECT
        Q_PROPERTY(QImage source WRITE setSource READ source)
        Q_PROPERTY(double pictureScale WRITE setPictureScale READ pictureScale)

    public:
        PictureItem(QQuickItem* parent = nullptr);

        void setSource(const QImage& image);
        void setPictureScale(double);
        const QImage& source() const;
        double pictureScale() const;

        void paint(QPainter * painter) override;

    private:
        QImage m_source;
        QImage m_processedSource;
        double m_scale;

        void prepareSource();
};

#endif
