#ifndef JSONTOBACKEND_HPP
#define JSONTOBACKEND_HPP

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
    };
}

#endif
