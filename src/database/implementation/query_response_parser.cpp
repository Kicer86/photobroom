
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "query_response_parser.hpp"


namespace
{
    void parseTags(Photo::DataDelta& data, const QJsonValue& jv)
    {
        const QJsonObject jo =jv.toObject();

        Tag::TagsList tags;

        if (jo.contains("date"))
            tags[Tag::Types::Date] = TagValue::fromRaw(jo["date"].toString(), Tag::ValueType::Date);
        if (jo.contains("time"))
            tags[Tag::Types::Time] = TagValue::fromRaw(jo["time"].toString(), Tag::ValueType::Time);
        if (jo.contains("event"))
            tags[Tag::Types::Event] = jo["event"].toString();

        data.insert<Photo::Field::Tags>(tags);
    }
}


namespace Query
{
    QString commonFragments()
    {
        return QStringLiteral(
            R"(
                fragment TagsFields on Tags {
                    date
                    time
                    event
                }
            )");
    }
}


namespace ResponseParser
{
    std::vector<Photo::DataDelta> parsePhotosQueryResponse(const QString& response)
    {
        std::vector<Photo::DataDelta> photos;

        const QJsonDocument doc = QJsonDocument::fromJson(response.toLocal8Bit());
        const QJsonObject rootObject = doc.object();
        const QJsonObject dataObject = rootObject["data"].toObject();
        const QJsonArray photosArray = dataObject["photos"].toArray();

        for (const auto& photoValue: photosArray)
        {
            const QJsonObject photoObject = photoValue.toObject();
            const QString id = photoObject["id"].toString();

            Photo::DataDelta photo(Photo::Id(id.toInt()));

            if (photoObject.contains("tags"))
                parseTags(photo, photoObject["tags"]);

            photos.push_back(photo);
        }

        return photos;
    }
}
