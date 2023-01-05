
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "query_response_parser.hpp"


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

            photos.push_back(photo);
        }

        return photos;
    }
}
