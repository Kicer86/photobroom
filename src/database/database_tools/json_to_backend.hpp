#ifndef JSONTOBACKEND_HPP
#define JSONTOBACKEND_HPP


#include <QJsonArray>
#include <QJsonObject>
#include <QString>


namespace Database
{
    struct IBackend;

    /**
    * @brief Read json and store it in backend
    */
    class JsonToBackend
    {
    public:
        JsonToBackend(IBackend &);

        void append(const QString &);

    private:
        IBackend& m_backend;

        void parse(const QByteArray &);
        void parse(const QJsonObject &);
        void parsePhotos(const QJsonArray &);
        void parsePhoto(const QJsonObject &);
    };
}

#endif
