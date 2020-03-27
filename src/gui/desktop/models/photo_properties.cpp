

#include "photo_properties.hpp"


Q_DECLARE_METATYPE(PhotoProperties)


PhotoProperties::PhotoProperties()
    : m_path()
    , m_height(0)
    , m_width(0)
{

}


PhotoProperties::PhotoProperties(const QString& path, int h, int w)
    : m_path(path)
    , m_height(h)
    , m_width(w)
{

}

