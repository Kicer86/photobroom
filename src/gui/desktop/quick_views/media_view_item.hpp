
#ifndef MEDIAVIEW_HPP
#define MEDIAVIEW_HPP

#include <core/icore_factory_accessor.hpp>
#include "amedia_item.hpp"

class MediaViewItem: public AMediaItem
{
public:
    MediaViewItem();

    void paint(QPainter *painter) override;

private:
    QImage m_image;
    ICoreFactoryAccessor* m_core;

    void reload(const Photo::Id &);
    void setImage(const QString &);
};

#endif
