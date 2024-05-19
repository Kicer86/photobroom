
#pragma once

#include <QRect>
#include <database/person_data.hpp>

namespace Database
{
    QRect decodeFaceLocation(QStringView);
    Person::Fingerprint decodeFingerprint(const QByteArray &);
}
