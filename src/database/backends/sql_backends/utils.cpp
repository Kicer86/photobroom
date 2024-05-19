
#include <QRegularExpression>
#include <QStringList>

#include "utils.hpp"


namespace Database
{
    namespace
    {
        void slice(auto view, auto condition, auto append, std::size_t limit = std::numeric_limits<std::size_t>::max())
        {
            qsizetype begin = 0;
            qsizetype size = view.size();
            std::size_t count = 0;

            for(qsizetype i = 0; i < size; i++)
            {
                if (condition(view[i]) || i + 1 == size)
                {
                    auto len = i - begin;

                    // special case for last element which does not end with a separator.
                    if (i + 1 == size)
                        len++;

                    append(view.sliced(begin, len));
                    begin = i + 1;
                    count++;

                    if (count >= limit)
                        break;
                }
            }
        }
    }


    QRect decodeFaceLocation(QStringView loc)
    {
        std::array<QStringView, 4> splitted;
        size_t count = 0;

        slice(loc, [](QChar c){return c < '0' || c > '9';}, [&](QStringView s){splitted[count++] = s;}, splitted.size());

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
        slice(f, [](QChar c){return c == ' ';}, [&](QByteArrayView s){splitted.push_back(s);});

        Person::Fingerprint fingerprint;
        fingerprint.reserve(splitted.size());

        for(const QByteArrayView& component: splitted)
            fingerprint.push_back(component.toDouble());

        return fingerprint;
    }

}
