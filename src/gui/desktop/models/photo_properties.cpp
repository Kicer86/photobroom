

#include "photo_properties.hpp"


Q_DECLARE_METATYPE(PhotoProperties)


PhotoProperties::PhotoProperties()
    : m_path()
    , m_size()
{

}


PhotoProperties::PhotoProperties(const QString& path, const QSize& size)
    : m_path(path)
    , m_size(size)
{

}
