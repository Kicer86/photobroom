
#include <QRegularExpression>
#include <QStringList>

#include "utils.hpp"


namespace Database
{
    QRect decodeFaceLocation(QStringView loc)
    {
        std::array<QStringView, 4> splitted;
        qsizetype begin = 0;
        size_t count = 0;
        qsizetype size = loc.size();
        for(qsizetype i = 0; i < size; i++)
        {
            if (loc[i] < '0' || loc[i] > '9' || i + 1 == size)
            {
                auto len = i - begin;

                // special case for last element which does not end with a separator.
                if (i + 1 == size)
                    len++;

                splitted[count] = loc.sliced(begin, len);
                count++;
                begin = i + 1;
            }

            if (count == 4)
                break;
        }

        if (count == 4)
        {
            const QRect location(splitted[0].toInt(),
                                 splitted[1].toInt(),
                                 splitted[2].toInt(),
                                 splitted[3].toInt());

            return location;
        }
        else
            return {};
    }


    Person::Fingerprint decodeFingerprint(QByteArrayView f)
    {
        std::vector<QByteArrayView> splitted;
        qsizetype begin = 0;
        size_t count = 0;
        qsizetype size = f.size();
        for(qsizetype i = 0; i < size; i++)
        {
            if (f[i] == ' ' || i + 1 == size)
            {
                auto len = i - begin;

                // special case for last element which does not end with a separator.
                if (i + 1 == size)
                    len++;

                splitted.push_back(f.sliced(begin, len));
                count++;
                begin = i + 1;
            }
        }

        Person::Fingerprint fingerprint;
        fingerprint.reserve(splitted.size());

        for(const QByteArrayView& component: splitted)
            fingerprint.push_back(component.toDouble());

        return fingerprint;
    }

}
