

#include <QDateTime>
#include "tags_utils.hpp"


namespace Tag
{
    std::chrono::milliseconds timestamp(const TagsList& tags)
    {
        qint64 timestamp = 0;

        const auto dateIt = tags.find(TagTypes::Date);

        if (dateIt != tags.end())
        {
            const QDate date = dateIt->second.getDate();
            const auto timeIt = tags.find(TagTypes::Time);
            const QTime time = timeIt != tags.end()? timeIt->second.getTime(): QTime();
            const QDateTime dateTime(date, time);

            timestamp = dateTime.toMSecsSinceEpoch();
        }

        return std::chrono::milliseconds(timestamp);
    }
}
