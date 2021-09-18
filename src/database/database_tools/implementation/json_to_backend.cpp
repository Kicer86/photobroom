
#include <QDate>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMap>
#include <QStringList>
#include <QVariant>

#include "../json_to_backend.hpp"
#include "database/ibackend.hpp"
#include "database/igroup_operator.hpp"


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

        const QJsonValue groups = obj.value("groups");
        if (photos.isArray())
            parseGroups(groups.toArray());
    }


    void JsonToBackend::parsePhotos(const QJsonArray& photos)
    {
        std::vector<Photo::DataDelta> photosList;
        std::vector<QString> ids;

        for(const QJsonValue& photo: photos)
            if (photo.isObject())
            {
                auto [photoDelta, id] = parsePhoto(photo.toObject());
                photosList.push_back(photoDelta);
                ids.push_back(id);
            }

        m_backend.addPhotos(photosList);

        // bind json ids with stored photos ids
        for(int i = 0; i < ids.size(); i++)
            if (ids[i].isEmpty() == false)
                m_idsMap.emplace(ids[i], photosList[i].getId());
    }


    std::pair<Photo::DataDelta, QString> JsonToBackend::parsePhoto(const QJsonObject& photo)
    {
        Photo::DataDelta delta;
        QString id;

        for(auto it = photo.constBegin(); it != photo.constEnd(); ++it)
        {
            if (it.key() == "checksum")
                delta.insert<Photo::Field::Checksum>(it.value().toString().toUtf8());
            else if (it.key() == "flags")
            {
                // TODO: implement
            }
            else if (it.key() == "geometry")
            {
                const auto geometryObj = it.value().toObject();
                const QSize geometry = parseGeometry(geometryObj);
                delta.insert<Photo::Field::Geometry>(geometry);
            }
            else if (it.key() == "tags")
            {
                const auto tags = it.value().toObject();
                const Tag::TagsList tagsList = parseTags(tags);
                delta.insert<Photo::Field::Tags>(tagsList);
            }
            else if (it.key() == "path")
                delta.insert<Photo::Field::Path>(it.value().toString());
            else if (it.key() == "id")
                id = it.value().toString();
            else
                throw std::invalid_argument("unexpected entry for photo");
        }

        return {delta, id};
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


    QSize JsonToBackend::parseGeometry(const QJsonObject& geometry)
    {
        QSize result;

        for(auto it = geometry.constBegin(); it != geometry.constEnd(); ++it)
        {
            const int value = it.value().toVariant().toInt();

            if (it.key() == "width")
                result.setWidth(value);
            else if (it.key() == "height")
                result.setHeight(value);
            else
                throw std::invalid_argument("unexpected entry for geometry");
        }

        return result;
    }


    void JsonToBackend::parseGroups(const QJsonArray& groups)
    {
        for(const QJsonValue& group: groups)
            if (group.isObject())
            {
                const Group groupInfo = parseGroup(group.toObject());
                const auto grpId = m_backend.groupOperator().addGroup(groupInfo.representative, ::Group::Type::Generic);

                std::vector<Photo::DataDelta> deltas;
                for(const auto& member: groupInfo.members)
                {
                    Photo::DataDelta delta(member);
                    delta.insert<Photo::Field::GroupInfo>(GroupInfo(grpId, GroupInfo::Role::Member));

                    deltas.push_back(delta);
                }

                m_backend.update(deltas);
            }
    }


    JsonToBackend::Group JsonToBackend::parseGroup(const QJsonObject& group)
    {
        const QString representative = group.value("representative").toString();
        const QJsonArray membersArray = group.value("members").toArray();

        std::vector<QString> members;
        std::transform(membersArray.begin(), membersArray.end(), std::back_inserter(members), [](const QJsonValue& value)
        {
            return value.toString();
        });

        auto idToPhotoId = [this](const QString& id) -> Photo::Id
        {
            return m_idsMap.at(id);
        };

        const Photo::Id representative_id = idToPhotoId(representative);
        std::vector<Photo::Id> memberIds;
        std::transform(members.begin(), members.end(), std::back_inserter(memberIds), idToPhotoId);

        return {.representative = representative_id, .members = memberIds};
    }
}
