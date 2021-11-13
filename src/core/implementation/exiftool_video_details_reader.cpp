
#include "exiftool_video_details_reader.hpp"

#include <cassert>

#include <QProcess>
#include <QRegularExpression>
#include <QDateTime>
#include <QTimeZone>



namespace ExiftoolUtils
{
    QStringList exiftoolOutput(const QString& exiftoolPath, const QString& filePath)
    {
        QProcess exiftool_process;
        exiftool_process.setProcessChannelMode(QProcess::MergedChannels);

        const QStringList exiftool_args = { filePath };

        exiftool_process.start(exiftoolPath, exiftool_args );
        bool status = exiftool_process.waitForFinished();
        status &= exiftool_process.exitCode() == 0;

        QStringList result;
        if (status)
            while(exiftool_process.canReadLine())
            {
                const QByteArray line = exiftool_process.readLine();
                result.append(line.constData());
            }

        return result;
    }

    std::map<QString, QString> parseOutput(const QStringList& entries)
    {
        std::map<QString, QString> pairs;

        for(const QString& entry: entries)
        {
            const auto colon = entry.indexOf(":");
            if (colon != -1)
            {
                const QString entryKeyRaw = entry.first(colon);
                const QString entryKey = entryKeyRaw.trimmed();
                const QString valueRaw = entry.sliced(colon + 1);
                const QString value = valueRaw.trimmed();

                pairs.emplace(entryKey, value);
            }
        }

        return pairs;
    }
}


ExiftoolVideoDetailsReader::ExiftoolVideoDetailsReader(const std::map<QString, QString>& entries)
    : m_entries(entries)
{

}


bool ExiftoolVideoDetailsReader::hasDetails() const
{
    return m_entries.empty() == false;
}


std::optional<QSize> ExiftoolVideoDetailsReader::resolutionOf() const
{
    std::optional<QSize> result;

    auto widthIt = m_entries.find("Image Width");
    auto heightIt = m_entries.find("Image Height");

    if (widthIt != m_entries.end() && heightIt != m_entries.end())
    {
        QSize size(widthIt->second.toInt(), heightIt->second.toInt());

        auto rotationIt = m_entries.find("Rotation");
        if (rotationIt != m_entries.end())
        {
            const int rotation = rotationIt->second.toInt();

            if (rotation == 90 || rotation == 270)
                size.transpose();
        }

        result = size;
    }

    return result;
}


std::optional<int> ExiftoolVideoDetailsReader::durationOf() const
{
    std::optional<int> result;

    auto durationIt = m_entries.find("Duration");

    if (durationIt != m_entries.end())
    {
        const QString& durationStr = durationIt->second;
        if (durationStr.indexOf("s") == -1)
        {
            const QTime durationTime = QTime::fromString(durationStr, "H:mm:ss");
            result = QTime(0, 0, 0).secsTo(durationTime);
        }
        else
        {
            const QString durationStrTrimmed = durationStr.chopped(2);   // remove " s"
            const QTime durationTime = QTime::fromString(durationStrTrimmed, "ss.z");
            result = QTime(0, 0, 0).secsTo(durationTime);
        }
    }

    return result;
}


std::optional<QDateTime> ExiftoolVideoDetailsReader::creationTime() const
{
    std::optional<QDateTime> result;

    auto datetimeIt = m_entries.find("Date/Time Original");

    if (datetimeIt != m_entries.end())
    {
        const QString& datetimeRaw = datetimeIt->second;
        const auto p = datetimeRaw.indexOf("+");             // crop timezone info
        const QString datetimeStr = datetimeRaw.first(p);
        const QDateTime datetime = QDateTime::fromString(datetimeStr, "yyyy:MM:dd hh:mm:ss");

        result = datetime;
    }
    else if ( (datetimeIt = m_entries.find("Create Date")) != m_entries.end())
    {
        const QString& datetimeStr = datetimeIt->second;
        QDateTime datetime = QDateTime::fromString(datetimeStr, "yyyy:MM:dd hh:mm:ss");
        datetime.setTimeZone(QTimeZone::utc());

        result = datetime.toLocalTime();
    }

    return result;
}
