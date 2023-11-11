
#ifndef EXIFTOOL_VIDEO_DETAILS_READER_HPP_INCLUDED
#define EXIFTOOL_VIDEO_DETAILS_READER_HPP_INCLUDED

#include <optional>

#include <QDateTime>
#include <QSize>
#include <QStringList>


namespace ExiftoolUtils
{
    QStringList exiftoolOutput(const QString& exiftoolPath, const QString& filePath);
    std::map<QString, QString> parseOutput(const QStringList &);
}


class ExiftoolVideoDetailsReader
{
    public:
        explicit ExiftoolVideoDetailsReader(const std::map<QString, QString> &);

        bool hasDetails() const;                        // checks if input data contain any usefull details

        std::optional<QSize> resolutionOf() const;
        std::optional<int> durationOf() const;          // video duration in seconds
        std::optional<QDateTime> creationTime() const;  // creation time from video metadata

    private:
        const std::map<QString, QString> m_entries;
};


#endif
