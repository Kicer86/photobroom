
#include <QRegularExpression>
#include <QStringList>

#include "utils.hpp"


namespace Database
{
    QRect decodeFaceLocation(const QString& l)
    {
        const QStringList location_list = l.split(QRegularExpression("[ ,x]"));
        const QRect location(location_list[0].toInt(),
                             location_list[1].toInt(),
                             location_list[2].toInt(),
                             location_list[3].toInt());

        return location;
    }


    Person::Fingerprint decodeFingerprint(const QByteArray& f)
    {
        const QList<QByteArray> splitted = f.split(' ');

        Person::Fingerprint fingerprint;
        fingerprint.reserve(splitted.size());

        for(const QByteArray& component: splitted)
            fingerprint.push_back(component.toDouble());

        return fingerprint;
    }

}
