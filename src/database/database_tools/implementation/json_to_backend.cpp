
#include <QJsonDocument>
#include <QJsonObject>
#include <QMap>
#include <QStringList>
#include <QVariant>

#include "../json_to_backend.hpp"
#include "database/photo_data.hpp"
#include "database/ibackend.hpp"


namespace Database
{
    JsonToBackend::JsonToBackend(Database::IBackend& backend)
        : m_backend(backend)
    {

    }


    void JsonToBackend::append(const QString& json)
    {
        parse(json.toUtf8());
    }


    void JsonToBackend::parse(const QByteArray& json)
    {
        QJsonDocument doc = QJsonDocument::fromJson(json);
        QJsonObject obj = doc.object();

        if (obj.isEmpty() == false)
            parse(obj);
    }


    void JsonToBackend::parse(const QJsonObject& obj)
    {
        const QJsonValue photos = obj.value("photos");
        if (photos.isArray())
            parsePhotos(photos.toArray());
    }


    void JsonToBackend::parsePhotos(const QJsonArray& photos)
    {
        for(const QJsonValue& photo: photos)
            if (photo.isObject())
                parsePhoto(photo.toObject());
    }


    void JsonToBackend::parsePhoto(const QJsonObject& photo)
    {
        const auto entries = photo.toVariantMap();

        Photo::DataDelta delta;

        auto it = entries.find("path");
        if (it != entries.end())
            delta.insert<Photo::Field::Path>(it->toString());

        std::vector photos = { delta };
        m_backend.addPhotos(photos);
    }
}
