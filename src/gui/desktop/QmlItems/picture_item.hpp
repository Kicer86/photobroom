
#ifndef IMAGE_ITEM_HPP
#define IMAGE_ITEM_HPP

#include <QImage>
#include <QQuickPaintedItem>
#include <QQmlEngine>

class PictureItem: public QQuickPaintedItem
{
        Q_OBJECT
        Q_PROPERTY(QImage source WRITE setSource READ source NOTIFY sourceChanged)
        QML_NAMED_ELEMENT(Picture)

    public:
        PictureItem(QQuickItem* parent = nullptr);

        void setSource(const QImage& image);
        const QImage& source() const;

        void paint(QPainter * painter) override;

    private:
        QImage m_source;

        bool validateInputs() const;

    signals:
        void sourceChanged();
};

#endif
