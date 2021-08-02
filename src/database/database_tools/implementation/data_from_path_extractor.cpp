
#include <QDate>

#include <core/tag.hpp>
#include "data_from_path_extractor.hpp"


DataFromPathExtractor::DataFromPathExtractor()
//            <  YEAR  >     <  MONTH >     <  DAY   >[   _      <  HOUR  >         < MINUTE >         < SECOND >]
    : m_expr("([0-9]{4})[.-]?([0-9]{2})[.-]?([0-9]{2})(?:[_ \\-]?([0-9]{2})[\\-.;_]?([0-9]{2})[\\-.;_]?([0-9]{2}))?")
{

}


Tag::TagsList DataFromPathExtractor::extract(const QString& path) const
{
    auto matchIt = m_expr.globalMatch(path);

    std::vector<QRegularExpressionMatch> matches;
    while (matchIt.hasNext())
    {
        QRegularExpressionMatch match = matchIt.next();
        matches.push_back(match);
    };

    // in case of more than one match, put the richer matches first
    std::sort(matches.begin(), matches.end(), [](const QRegularExpressionMatch& lhs, const QRegularExpressionMatch& rhs) {
        return lhs.lastCapturedIndex() > rhs.lastCapturedIndex();
    });

    Tag::TagsList tags;

    for(const auto& match: matches)
    {
        const QStringList captured = match.capturedTexts();

        const int y = captured[1].toInt();
        const int m = captured[2].toInt();
        const int d = captured[3].toInt();
        const QDate date(y, m, d);

        if (date.isValid())
        {
            tags.emplace(TagTypes::Date, date);

            if (captured.size() == 7)
            {
                const int hh = captured[4].toInt();
                const int mm = captured[5].toInt();
                const int ss = captured[6].toInt();
                const QTime time(hh, mm, ss);

                if (time.isValid())
                    tags.emplace(TagTypes::Time, time);
            }

            break;
        }
    }

    return tags;
}
