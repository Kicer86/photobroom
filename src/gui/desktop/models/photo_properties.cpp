

#include "photo_properties.hpp"


Q_DECLARE_METATYPE(PhotoProperties)


PhotoProperties::PhotoProperties()
    : m_path()
    , m_size()
    , m_id()
{

}


PhotoProperties::PhotoProperties(const QString& path, const QSize& size, const Photo::Id& id)
    : m_path(path)
    , m_size(size)
    , m_id(id)
{

}
