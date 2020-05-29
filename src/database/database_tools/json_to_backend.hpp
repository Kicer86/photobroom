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
        IBackend& m_backend;

        void parse(const QByteArray &);
        void parse(const QJsonObject &);
        void parsePhotos(const QJsonArray &);
        Photo::DataDelta parsePhoto(const QJsonObject &);
        Tag::TagsList parseTags(const QJsonObject &);
    };
}

#endif
