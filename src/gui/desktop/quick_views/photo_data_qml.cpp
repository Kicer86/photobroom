

#include "photo_data_qml.hpp"


PhotoDataQml::PhotoDataQml(QObject* p)
    : QObject(p)
{
}


PhotoDataQml::PhotoDataQml(const Photo::Data& photo)
    : m_photo(photo)
{

}


QVariantMap PhotoDataQml::getFlags() const
{
    QVariantMap result;

    for(const auto& flag: m_photo.flags)
    {
        const QString id = QString::number(static_cast<int>(flag.first));
        result[id] = flag.second;
    }

    return result;
}


const Photo::Data& PhotoDataQml::getPhotoData() const
{
    return m_photo;
}


void PhotoDataQml::setPhotoData(const Photo::Data& photo)
{
    m_photo = photo;

    emit photoDataChanged(m_photo);
    emit flagsChanged(getFlags());
    emit isGroupChanged(isGroup());
}


bool PhotoDataQml::isGroup() const
{
    return m_photo.groupInfo.role == GroupInfo::Representative;
}
