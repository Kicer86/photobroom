
#include <QDate>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMap>
#include <QStringList>
#include <QVariant>

#include "../json_to_backend.hpp"
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
        std::vector<Photo::DataDelta> photosList;

        for(const QJsonValue& photo: photos)
            if (photo.isObject())
            {
                const auto photoDelta = parsePhoto(photo.toObject());
                photosList.push_back(photoDelta);
            }

        m_backend.addPhotos(photosList);
    }


    Photo::DataDelta JsonToBackend::parsePhoto(const QJsonObject& photo)
    {
        Photo::DataDelta delta;

        for(auto it = photo.constBegin(); it != photo.constEnd(); ++it)
        {
            if ( it.key() == "path")
                delta.insert<Photo::Field::Path>(it.value().toString());
            else if (it.key() == "tags")
            {
                const auto tags = it.value().toObject();
                const Tag::TagsList tagsList = parseTags(tags);
                delta.insert<Photo::Field::Tags>(tagsList);
            }
            else
                throw std::invalid_argument("unexpected entry for photo");
        }

        return delta;
    }


    Tag::TagsList JsonToBackend::parseTags(const QJsonObject& tag)
    {
        Tag::TagsList tagsList;

        for(auto it = tag.constBegin(); it != tag.constEnd(); ++it)
        {
            const QString value = it.value().toVariant().toString();
            if (value.isEmpty())
                continue;

            if (it.key() == "date")
                tagsList[TagTypes::Date] = QDate::fromString(value, Qt::ISODate);
            else if (it.key() == "time")
                tagsList[TagTypes::Time] = QTime::fromString(value);
            else if (it.key() == "event")
                tagsList[TagTypes::Event] = value;
            else if (it.key() == "place")
                tagsList[TagTypes::Place] = value;
            else
                throw std::invalid_argument("unexpected entry for tag");
        }

        return tagsList;
    }
}
