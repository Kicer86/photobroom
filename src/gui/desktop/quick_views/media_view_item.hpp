
#ifndef MEDIAVIEW_HPP
#define MEDIAVIEW_HPP

#include "amedia_item.hpp"

class MediaViewItem: public AMediaItem
{
public:
    MediaViewItem();

    void paint(QPainter *painter) override;

private:
    QImage m_image;

    void reload(const Photo::Id &);
    void setImage(const QString &);
};

#endif
