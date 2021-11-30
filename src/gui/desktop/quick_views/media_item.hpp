
#ifndef MEDIAITEM_HPP
#define MEDIAITEM_HPP

#include <QQuickPaintedItem>


class MediaItem: public QQuickPaintedItem
{
public:
    void paint(QPainter* painter) override;
};

#endif
