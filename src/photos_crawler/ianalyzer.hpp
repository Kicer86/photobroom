
#ifndef ANALYZER_IANALYZER_HPP
#define ANALYZER_IANALYZER_HPP

#include "photos_crawler_export.h"

class QString;

struct PHOTOS_CRAWLER_EXPORT IAnalyzer
{
    virtual ~IAnalyzer() = default;

    virtual bool isMediaFile(const QString &) = 0;
};

#endif
