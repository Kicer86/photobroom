

#include <database/photo_utils.hpp>

#include "photo_data_qml.hpp"


PhotoDataQml::PhotoDataQml(QObject* p)
    : QObject(p)
{
}


PhotoDataQml::PhotoDataQml(const Photo::DataDelta& photo)
    : m_photo(photo)
{

}


QVariantMap PhotoDataQml::getFlags() const
{
    QVariantMap result;

    if (m_photo.getId().valid())
        for(const auto& flag: m_photo.get<Photo::Field::Flags>())
        {
            const QString id = QString::number(static_cast<int>(flag.first));
            result[id] = flag.second;
        }

    return result;
}


const Photo::DataDelta& PhotoDataQml::getPhotoData() const
{
    return m_photo;
}


void PhotoDataQml::setPhotoData(const Photo::DataDelta& photo)
{
    m_photo = photo;

    emit photoDataChanged(m_photo);
    emit flagsChanged(getFlags());
    emit isGroupChanged(isGroup());
}


bool PhotoDataQml::isGroup() const
{
    return m_photo.getId().valid()? Photo::is<GroupInfo::Representative>(m_photo) : false;
}
