
#pragma once

#include <QRect>
#include <core/filesystem.hpp>
#include <database/person_data.hpp>

namespace Database
{
    QRect decodeFaceLocation(QStringView);
    Person::Fingerprint decodeFingerprint(QByteArrayView);

    QString locationToString(const Filesystem::Location &);
    Filesystem::Location stringToLocation(const QString &);
}
