
#include <QJsonDocument>

#include "../json_to_backend.hpp"


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
        QJsonDocument doc = QJsonDocument::fromBinaryData(json);
    }
}
