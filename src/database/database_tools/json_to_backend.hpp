#ifndef JSONTOBACKEND_HPP
#define JSONTOBACKEND_HPP


#include <QJsonArray>
#include <QJsonObject>
#include <QString>

#include <core/tag.hpp>
#include "database/photo_data.hpp"
#include "database_export.h"


namespace Database
{
    struct IBackend;

    /**
    * @brief Read json and store it in backend
    */
    class DATABASE_EXPORT JsonToBackend
    {
    public:
        explicit JsonToBackend(IBackend &);

        void append(const QString &);

    private:
        struct Group
        {
            Photo::Id representative;
            std::vector<Photo::Id> members;
        };

        std::map<QString, Photo::Id> m_idsMap;
        IBackend& m_backend;

        void parse(const QByteArray &);
        void parse(const QJsonObject &);
        void parsePhotos(const QJsonArray &);
        std::pair<Photo::DataDelta, QString> parsePhoto(const QJsonObject &);
        Tag::TagsList parseTags(const QJsonObject &);
        QSize parseGeometry(const QJsonObject &);
        void parseGroups(const QJsonArray &);
        Group parseGroup(const QJsonObject &);
    };
}

#endif
