
#pragma once

#include <QRect>
#include <database/person_data.hpp>

namespace Database
{
    QRect decodeFaceLocation(const QString &);
    Person::Fingerprint decodeFingerprint(const QByteArray &);
}
