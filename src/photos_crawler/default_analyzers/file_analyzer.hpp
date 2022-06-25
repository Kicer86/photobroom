
#ifndef ANALYZER_FILE_ANALYZER
#define ANALYZER_FILE_ANALYZER

#include <photos_crawler/ianalyzer.hpp>
#include "photos_crawler_export.h"


class PHOTOS_CRAWLER_EXPORT FileAnalyzer final: public IAnalyzer
{
    public:
        FileAnalyzer();
        virtual ~FileAnalyzer();

        virtual bool isMediaFile(const QString &) override;
};

#endif
